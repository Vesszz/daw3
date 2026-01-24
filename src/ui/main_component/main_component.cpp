#include "main_component.h"

MainComponent::MainComponent() {
    addAndMakeVisible(m_pianoroll);
    setSize(1200, 800);
}

MainComponent::~MainComponent() {
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized() {
    m_pianoroll.setBounds(getLocalBounds());
}
