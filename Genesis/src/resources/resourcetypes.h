#pragma once

enum class ResourceType
{
    Texture,
    Font,
    Sound,
    Video,
    Shader,

    Unknown
};

enum class ResourceState
{
    Unloaded,
    PreloadPending,
    Preloading,
    Preloaded,
    Loaded
};