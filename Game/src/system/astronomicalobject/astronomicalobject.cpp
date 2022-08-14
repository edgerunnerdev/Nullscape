// Copyright 2022 Pedro Nunes
//
// This file is part of Hyperscape.
//
// Hyperscape is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Hyperscape is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hyperscape. If not, see <http://www.gnu.org/licenses/>.

#include "system/astronomicalobject/astronomicalobject.hpp"

// clang-format off
#include <externalheadersbegin.hpp>
#include <glm/trigonometric.hpp>
#include <externalheadersend.hpp>
// clang-format on

#include "system/astronomicalobject/orbit.hpp"
#include "ui2.hpp"

namespace Hyperscape
{

AstronomicalObject::AstronomicalObject(SystemRandomEngine& randomEngine, const std::string& name, const glm::vec2& coordinates)
    : SignalSource(randomEngine)
    , m_Coordinates(coordinates)
    , m_Name(name)
{
    m_RandomEngine = LocalRandomEngine(randomEngine());
}

AstronomicalObject::AstronomicalObject(SystemRandomEngine& randomEngine, const std::string& name, OrbitUniquePtr pOrbit, float theta) 
    : SignalSource(randomEngine)
    , m_pOrbit(std::move(pOrbit))
    , m_Name(name)
{
    m_Coordinates = m_pOrbit->At(theta);
}

AstronomicalObject::~AstronomicalObject()
{
}

void AstronomicalObject::CanvasRender(const ImVec2& canvasTopLeft, const ImVec2& canvasBottomRight, const ImVec2& canvasOffset)
{
    if (m_pOrbit != nullptr)
    {    
        const float oneDegree = glm::radians(1.0f);
        for (int i = 0; i < m_OrbitPoints.size(); ++i)
        {
            m_OrbitPoints[i] = UI2::ToCanvasCoordinates(canvasTopLeft, canvasBottomRight, canvasOffset, m_pOrbit->At(i * oneDegree));
        }
        ImGui::GetWindowDrawList()->AddPolyline(m_OrbitPoints.data(), m_OrbitPoints.size(), IM_COL32(255, 255, 255, 80), ImDrawFlags_Closed, 1.25f);
    }
}

void AstronomicalObject::UpdateDebugUI() 
{

}

const std::string& AstronomicalObject::GetName() const 
{
    return m_Name;
}

LocalRandomEngine& AstronomicalObject::GetRandomEngine() 
{
    return m_RandomEngine;
}

const glm::vec2& AstronomicalObject::GetSignalCoordinates() const
{
    return m_Coordinates;
}

void AstronomicalObject::AddBlackBodySignal(double temperature) 
{
    // Calculate the spectral radiance assuming the astronomical object is a black body, using Planck's law.
    // https://www.fxsolver.com/browse/formulas/Planck's+law+(+by+wavelength)
    Wavelength onenm = 1.0_nm;
    static const double h = 6.62607015 * pow(10.0, -34.0); // Planck's constant.
    static const double c = 299792458.0; // Speed of light.
    static const double e = 2.71828182845904523536;
    static const double kb = 1.3806488 * pow(10, -23.0); // Boltzmann constant.
    static const double unitConversion = pow(10.0, -12.0); // From watt per steradian per square metre per metre to kilowatt per steradian per square metre per nanometre.

    for (size_t i = 0; i < m_SignalData.sNumEntries; i++)
    {
        const double wavelength = m_SignalData.Wavelengths[i];
        if (wavelength > 0.0)
        {
            const double radiance = (2.0 * h * c * c) / pow(wavelength, 5.0) / (pow(e, (h * c) / (wavelength * kb * temperature)) - 1.0) * unitConversion;
            m_SignalData.Intensities[i] = radiance;
        }
    }

    // Add some noise to the signal.
    std::uniform_real_distribution<float> dist(0.9f, 1.1f);
    for (size_t i = 0; i < m_SignalData.sNumEntries; i++)
    {
        m_SignalData.Intensities[i] *= dist(m_RandomEngine);
    }
}

} // namespace Hyperscape