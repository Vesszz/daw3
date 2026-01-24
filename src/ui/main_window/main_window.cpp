#include "main_window.h"

MainWindow::MainWindow(const juce::String& windowTitle)
    : DocumentWindow(windowTitle,
                    juce::Desktop::getInstance().getDefaultLookAndFeel()
                    .findColour(juce::ResizableWindow::backgroundColourId),
                    DocumentWindow::allButtons) {
    setUsingNativeTitleBar(true);
    
    m_main_component = std::make_unique<MainComponent>();
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
