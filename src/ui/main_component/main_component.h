#pragma once
#include "../../juce_header.h"
#include "../pianoroll/pianoroll.h"

class MainComponent : public juce::Component {
public:
    MainComponent();
    ~MainComponent() override;
    void paint(juce::Graphics&) override;
    void resized() override;
    
private:
    PianoRoll m_pianoroll;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
