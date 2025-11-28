#include "instrumental_track.h"
#include <memory>

std::unique_ptr<juce::AudioPluginInstance> InstrumentalTrack::load_plugin(juce::File filename, float sampleRate, int blockSize)
{
    juce::AudioPluginFormatManager formatManager;
    formatManager.addDefaultFormats();

    juce::OwnedArray<juce::PluginDescription> foundTypes;

    for (auto* fmt : formatManager.getFormats())
    {
        if (fmt == nullptr) continue;
        fmt->findAllTypesForFile(foundTypes, filename.getFullPathName());
        if (foundTypes.size() > 0)
            break;
    }
    
    if (foundTypes.size() == 0)
    {
        return nullptr;
    }

    juce::String errorMessage;
    std::unique_ptr<juce::AudioPluginInstance> inst =
        formatManager.createPluginInstance(*foundTypes[0], sampleRate, blockSize, errorMessage);
    if (!inst)
    {
        return nullptr; // TODO: show errorMessage
    }
    return inst;
}


InstrumentalTrack::InstrumentalTrack(std::string name, std::unique_ptr<juce::AudioPluginInstance> a_p_i)
    : m_name(name), m_audio_plugin_instance(std::move(a_p_i)), m_midi_message_sequence() {}

const juce::String InstrumentalTrack::info() {
    return m_audio_plugin_instance.get()->getName(); // TODO: good info
}
