#include "daw3.h"
#include <exception>
#include <stdexcept>

DAW3::DAW3() {
    try {
        m_audio_engine = std::make_unique<AudioEngine>();
    } catch (std::exception e) {
        throw std::runtime_error("runtime error: failed to initialise audio engine: " + std::string(e.what()));
    }
    m_ui = std::make_unique<UI>();
}

void DAW3::init_ui() {
    m_ui.get()->init();
}

void DAW3::run_ui() {
    m_ui.get()->run();
}
