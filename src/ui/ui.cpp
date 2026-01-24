#include "ui.h"
#include "main_window/main_window.h"
#include <exception>
#include <memory>
#include <type_traits>

void quit() {
    return;
}

void UI::init() {
    m_juce_initialiser_gui = std::make_unique<juce::ScopedJuceInitialiser_GUI>();
    m_main_window = std::make_unique<MainWindow>("Empty Window Example");
}

void UI::run() {
    juce::MessageManager::getInstance()->runDispatchLoop();
    quit();
}
