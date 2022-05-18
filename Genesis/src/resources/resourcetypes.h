#pragma once

enum class ResourceType
{
	Texture,
	Texture3D,
	Font,
	Sound,
	Video,

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