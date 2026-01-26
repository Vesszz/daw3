#include "daw3.h"
#include <exception>
#include <stdexcept>
#include "../queue/queue.h"
#include "../queue/midi_command.h"
#include "juce_audio_devices/juce_audio_devices.h"

DAW3::DAW3(std::shared_ptr<Queue<MidiCommand, 1024>> q) {
    try {
        m_audio_engine = std::make_unique<AudioEngine>(std::move(q));
    } catch (std::exception e) {
        throw std::runtime_error("runtime error: failed to initialise audio engine: " + std::string(e.what()));
    }
    m_ui = std::make_unique<UI>();
}
void DAW3::start_audio_engine() {
    m_audio_engine.get()->start();
}


void DAW3::init_ui(std::shared_ptr<Queue<MidiCommand, 1024>> q) {
    m_ui.get()->init(q, m_audio_engine.get()->getTrackHandler());
}

void DAW3::run_ui() {
    m_ui.get()->run();
}
