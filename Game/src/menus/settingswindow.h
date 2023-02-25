// Copyright 2021 Pedro Nunes
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

#pragma once

#include <any>

#include "ui/types.fwd.h"
#include "ui/window.h"

namespace Hyperscape
{

class SettingsWindow : public UI::Window
{
public:
	SettingsWindow();
	virtual void Reset() override;

private:
	UI::ElementSharedPtr CreateAudioPanel();
	UI::ElementSharedPtr CreateVideoPanel();
	UI::ElementSharedPtr CreateGameplayPanel();

	UI::SliderSharedPtr m_pMasterVolumeSlider;
	UI::SliderSharedPtr m_pMusicVolumeSlider;
	UI::SliderSharedPtr m_pSFXVolumeSlider;
	UI::CheckboxSharedPtr m_pPostProcessBleachBypassCheckbox;
	UI::CheckboxSharedPtr m_pPostProcessGlowCheckbox;
	UI::CheckboxSharedPtr m_pPostProcessVignetteCheckbox;
	UI::CheckboxSharedPtr m_pOutlinesCheckbox;
	UI::CheckboxSharedPtr m_pFireToggleCheckbox;

    void SetMasterVolume( float value );
    void SetMusicVolume( float value );
    void SetSFXVolume( float value );
};

} // namespace Hyperscape
