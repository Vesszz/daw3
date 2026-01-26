#include "ui.h"
#include "main_window/main_window.h"
#include <memory>
#include "../audio_engine/track_handler/track_handler.h"

void quit() {
    return;
}

void UI::init(std::shared_ptr<Queue<MidiCommand, 1024>> q, std::unique_ptr<TrackHandler> th) {
    m_queue = std::move(q);
    m_juce_initialiser_gui = std::make_unique<juce::ScopedJuceInitialiser_GUI>();
    m_main_window = std::make_unique<MainWindow>("DAW3", m_queue, std::move(th));
}

void UI::run() {
    juce::MessageManager::getInstance()->runDispatchLoop();
    quit();
}
