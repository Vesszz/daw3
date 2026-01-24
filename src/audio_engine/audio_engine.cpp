#include "audio_engine.h"
#include <stdexcept>

AudioEngine::AudioEngine() {
    m_audio_device_manager = std::make_unique<juce::AudioDeviceManager>();
    juce::String error = m_audio_device_manager->initialise(0, 2, nullptr, true);
    if (error != "") {
        throw std::runtime_error("runtime error: failed to initialise audiodevicemanager in daw3 constructor: " + error.toStdString());
    }
}
