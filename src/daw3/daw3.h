#pragma once
#include <memory>
#include "../audio_engine/audio_engine.h"
#include "../ui/ui.h"

class DAW3 {
    public:
        DAW3();
        ~DAW3() = default;
        void init_ui();
        void run_ui();
        void add_track(InstrumentalTrack&&);
        void add_track(AudioTrack&&);
    private:
        std::unique_ptr<UI> m_ui;
        std::unique_ptr<AudioEngine> m_audio_engine;
};
