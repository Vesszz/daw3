#pragma once

#include "../../juce_header.h"
#include "../main_component/main_component.h"
#include "../../queue/queue.h"
#include "../../queue/midi_command.h"
#include "../../audio_engine/track_handler/track_handler.h"

class MainWindow : public juce::DocumentWindow {
    public:
        MainWindow(const juce::String&, std::shared_ptr<Queue<MidiCommand, 1024>>, std::unique_ptr<TrackHandler>);
        ~MainWindow() override;
        void closeButtonPressed() override;
    private:
        std::unique_ptr<MainComponent> m_main_component;
        std::shared_ptr<Queue<MidiCommand, 1024>> m_queue;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
