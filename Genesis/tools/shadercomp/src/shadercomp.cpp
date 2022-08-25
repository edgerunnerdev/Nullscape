// Copyright 2022 Pedro Nunes
//
// This file is part of Genesis.
//
// Genesis is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Genesis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Genesis. If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <externalheadersbegin.hpp>
#include <nlohmann/json.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include <GL/glew.h>
#include <log.hpp>
#include <fstream>
#include "shadercomp.hpp"

namespace Genesis::ResComp
{

ShaderComp::ShaderComp()
    : m_pWindow(nullptr)
    , m_pContext(nullptr)
{
}

ShaderComp::~ShaderComp() 
{
    if (m_pWindow != nullptr)
    {
        SDL_DestroyWindow(m_pWindow);
        SDL_GL_DeleteContext(m_pContext);
        SDL_Quit();
    }
}

bool ShaderComp::Initialize(int argc, char** argv) 
{
    if (ResComp::Initialize(argc, argv) == false)
    {
        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        Log::Error() << SDL_GetError();
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); // OpenGL core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);                          // OpenGL 3+
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);                          // OpenGL 3.3

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    m_pWindow = SDL_CreateWindow("ShaderComp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 200, 200, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    m_pContext = SDL_GL_CreateContext(m_pWindow);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        Log::Error() << "GLEW initialisation failed: " << glewGetErrorString(err);
        return false;
    }
    else
    {
        return true;
    }
}

int ShaderComp::Run()
{
    std::filesystem::path targetPath = GetDataDir() / std::filesystem::relative(GetFile(), GetAssetsDir()).remove_filename();
    std::filesystem::create_directories(targetPath);

    using namespace nlohmann;
    json j;

    std::filesystem::path sourceFile;
    std::ifstream file(GetFile());
    if (file.good())
    {
        json j;
        file >> j;

        json::iterator it = j.find("source");
        if (it != j.end() && it->is_string())
        {
            std::filesystem::path filePath(GetFile());
            sourceFile = filePath.remove_filename() / it->get<std::string>();
        }
        file.close();
    }

    if (std::filesystem::exists(sourceFile) == false)
    {
        Log::Error() << "Source file " << sourceFile << "doesn't exist.";
        return -1;
    }

    if (ValidateShader(sourceFile) == false)
    {
        Log::Error() << "Shader validation for " << sourceFile << " failed.";
        return -1;
    }

    std::filesystem::path targetFile = targetPath / sourceFile.filename();
    bool fileCopied = std::filesystem::copy_file(sourceFile, targetFile, std::filesystem::copy_options::overwrite_existing);
    if (fileCopied)
    {
        OnResourceBuilt(GetFile(), sourceFile, targetFile);
        return 0;
    }
    else
    {
        OnAssetCompilationFailed(GetFile(), "File copy failed.");
        return -1;
    }
}

bool ShaderComp::ValidateShader(const std::filesystem::path& path) 
{
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::string shaderCode;
    std::ifstream shaderFile(path);

    if (shaderFile.is_open())
    {
        std::string line = "";
        while (getline(shaderFile, line))
        {
            shaderCode += "\n" + line;
        }

        shaderFile.close();
    }
    else
    {
        Log::Error() << "Couldn't open " << path << ".";
        return false;
    }

    const std::string vertexShaderCode = "#version 330\n#define VERTEX_PROGRAM\n#line 0\n" + shaderCode;
    const std::string fragmentShaderCode = "#version 330\n#define FRAGMENT_PROGRAM\n#line 0\n" + shaderCode;

    GLint compilationSuccessful = GL_FALSE;
    int infoLogLength = 0;

    // Compile Vertex Shader
    const char* vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer, nullptr);
    glCompileShader(vertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compilationSuccessful);
    if (compilationSuccessful == GL_FALSE)
    {
        glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderID, infoLogLength, nullptr, &vertexShaderErrorMessage[0]);
        Log::Error()  << vertexShaderErrorMessage.data();
        return false;
    }

    // Compile Fragment Shader
    const char* fragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, nullptr);
    glCompileShader(fragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compilationSuccessful);
    if (compilationSuccessful == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, nullptr, &fragmentShaderErrorMessage[0]);
        Log::Error() << fragmentShaderErrorMessage.data();
        return false;
    }

    // Link the program
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShaderID);
    glAttachShader(programHandle, fragmentShaderID);
    glLinkProgram(programHandle);

    // Check the program
    GLint linkSuccessful = GL_FALSE;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccessful);
    if (linkSuccessful == GL_FALSE)
    {
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programHandle, infoLogLength, nullptr, &programErrorMessage[0]);
        Log::Error() << &programErrorMessage[0];
        return false;
    }

    glDetachShader(programHandle, vertexShaderID);
    glDetachShader(programHandle, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return true;
}

} // namespace Genesis::ResComp
