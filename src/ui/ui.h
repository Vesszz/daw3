#pragma once

#include <memory>

#include "main_window/main_window.h"
#include "../../external/JUCE/modules/juce_events/juce_events.h"
#include "../queue/queue.h"
#include "../queue/midi_command.h"
#include "../audio_engine/track_handler/track_handler.h"

class UI {
    public:
        void init(std::shared_ptr<Queue<MidiCommand, 1024>>, std::unique_ptr<TrackHandler>);
        void run();
    private:
        std::shared_ptr<Queue<MidiCommand, 1024>> m_queue;
        std::unique_ptr<juce::ScopedJuceInitialiser_GUI> m_juce_initialiser_gui;
        std::unique_ptr<MainWindow> m_main_window;
};
