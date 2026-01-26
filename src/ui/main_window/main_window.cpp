#include "main_window.h"

MainWindow::MainWindow(const juce::String& windowTitle, std::shared_ptr<Queue<MidiCommand,1024>> q, std::unique_ptr<TrackHandler> th)
    : DocumentWindow(windowTitle,
                    juce::Desktop::getInstance().getDefaultLookAndFeel()
                    .findColour(juce::ResizableWindow::backgroundColourId),
                    DocumentWindow::allButtons) {
    m_queue = std::move(q);
    setUsingNativeTitleBar(true);
    
    m_main_component = std::make_unique<MainComponent>(q, std::move(th));
    setContentOwned(m_main_component.get(), true);
    
    centreWithSize(1200, 800);
    setVisible(true);
    setResizable(true, true);
}

MainWindow::~MainWindow() {
    m_main_component.reset();
}

void MainWindow::closeButtonPressed() {
    juce::JUCEApplicationBase::quit();
}
