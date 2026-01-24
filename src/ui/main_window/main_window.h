#pragma once

#include "../../juce_header.h"

class MainWindow : public juce::DocumentWindow {
    public:
        MainWindow(const juce::String&);
        ~MainWindow() override;
        void closeButtonPressed() override;
    private:
        std::unique_ptr<juce::Component> m_main_component;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
