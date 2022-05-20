// Copyright 2015 Pedro Nunes
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

#include "sound/private/fmod/soundmanager.h"

#include "sound/private/null/soundmanager.h"
#include "sound/private/soundinstanceimpl.h"
#include "sound/private/soundmanagerimpl.h"
#include "sound/soundmanager.h"

namespace Genesis::Sound
{

SoundManager::SoundManager()
{
#if defined USE_FMOD
    m_pImpl = std::make_unique<Private::FMOD::SoundManager>();
#elif defined USE_SDL_MIXER
    m_pImpl = std::make_unique<Private::SDLMixer::SoundManager>();
#else
    m_pImpl = std::make_unique<Private::Null::SoundManager>();
#endif
}

SoundManager::~SoundManager() {}

TaskStatus SoundManager::Update(float delta)
{
    m_pImpl->Update(delta);
    return TaskStatus::Continue;
}

SoundInstanceSharedPtr SoundManager::CreateSoundInstance(ResourceSound* pResourceSound)
{
    return m_pImpl->CreateSoundInstance(pResourceSound);
}

void SoundManager::SetPlaylist(ResourceSound* pResourceSound, const std::string& startingTrack /* = "" */, bool shuffle /* = false */)
{
    m_pImpl->SetPlaylist(pResourceSound, startingTrack, shuffle);
}

ResourceSound* SoundManager::GetPlaylistResource() const
{
    return m_pImpl->GetPlaylistResource();
}

SoundInstanceSharedPtr SoundManager::GetCurrentSong() const
{
    return m_pImpl->GetCurrentSong();
}

const SoundInstanceList& SoundManager::GetSoundInstances() const
{
    return m_pImpl->GetSoundInstances();
}

void SoundManager::SetListener(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up)
{
    m_pImpl->SetListener(position, velocity, forward, up);
}

glm::vec3 SoundManager::GetListenerPosition() const
{
    return m_pImpl->GetListenerPosition();
}

int SoundManager::GetActiveSoundCount() const
{
    return m_pImpl->GetActiveSoundCount();
}

} // namespace Genesis::Sound
