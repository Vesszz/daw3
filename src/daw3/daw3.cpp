#include "daw3.h"
#include <memory>
#include <stdexcept>

DAW3::DAW3() {
    m_audio_device_manager = std::make_unique<juce::AudioDeviceManager>();
    juce::String error = m_audio_device_manager->initialise(0, 2, nullptr, true);
    if (error != "") {
         throw std::runtime_error("runtime error: failed to initialise audiodevicemanager in daw3 constructor: " + error.toStdString());
    }
}


juce::AudioDeviceManager& DAW3::getAudioDeviceManager() {
    return *m_audio_device_manager;
}

void DAW3::add_track(InstrumentalTrack&& it) {
    m_track_handler->add_track(std::move(it));
}
