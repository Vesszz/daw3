#pragma once
#include "juce_header.h"
#include "../external/JUCE/modules/juce_audio_processors/juce_audio_processors.h"
#include "../external/JUCE/modules/juce_audio_basics/juce_audio_basics.h"

class Sound {
    public:
        Sound();
        static juce::AudioBuffer<float> load_from_file(const juce::File& file);
        void add_audio_plugin_instance(const juce::AudioPluginInstance);
        ~Sound();
    private:
        double m_sample_rate;
        SampleTime m_time;
        std::string m_name;
        juce::File m_file;
        juce::AudioBuffer<float> m_original_audio_buffer;
        juce::AudioBuffer<float> m_processed_audio_buffer;
        juce::AudioProcessorGraph m_audio_processor_graph;
};
