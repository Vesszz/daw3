#pragma once

#include <memory>

#include "main_window/main_window.h"
#include "pianoroll/pianoroll.h"
#include "../../external/JUCE/modules/juce_events/juce_events.h"

class UI {
    public:
        void init();
        void run();
    private:
        std::unique_ptr<juce::ScopedJuceInitialiser_GUI> m_juce_initialiser_gui;
        std::unique_ptr<MainWindow> m_main_window;
};
