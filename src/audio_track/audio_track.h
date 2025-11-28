#pragma once

#include "../sound/sound.h"

class AudioTrack {
    private:
        std::string m_name;
        std::vector<std::pair<Sound, SampleTime>> m_sound_sequence;    
};
