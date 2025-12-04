#include "instrumental_track.h"
#include <memory>
#include "../../external/JUCE/modules/juce_audio_formats/juce_audio_formats.h"

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


// TODO: refactor this plz

void InstrumentalTrack::render_midifile_into_wav(std::string midiPath,std::string wavPath) {
    juce::File midiFile(midiPath);
    juce::FileInputStream midiStream(midiFile);
    if (!midiStream.openedOk())
        throw std::runtime_error("Failed to open MIDI file");

    juce::MidiFile mf;
    if (!mf.readFrom(midiStream))
        throw std::runtime_error("Failed to read MIDI file");

    mf.convertTimestampTicksToSeconds();

    juce::MidiMessageSequence fullSequence;
    for (int i = 0; i < mf.getNumTracks(); i++)
        fullSequence.addSequence(*mf.getTrack(i), 0.0);

    fullSequence.updateMatchedPairs();

    auto* plugin = m_audio_plugin_instance.get();
    if (!plugin)
        throw std::runtime_error("Audio plugin is null");

    double sampleRate = 44100.0;
    int blockSize     = 512;

    plugin->prepareToPlay(sampleRate, blockSize);

    int numChannels = plugin->getTotalNumOutputChannels();

    juce::File outFile(wavPath);
    outFile.deleteFile();

    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer(wavFormat.createWriterFor(
        outFile.createOutputStream().release(),
        sampleRate,
        (unsigned int)numChannels,
        16,
        {},
        0
    ));

    if (!writer)
        throw std::runtime_error("Failed to create WAV writer");

    juce::AudioBuffer<float> audioBuffer(numChannels, blockSize);
    juce::MidiBuffer midiBuffer;

    double lengthSeconds = fullSequence.getEndTime();
    int totalSamples = (int)(lengthSeconds * sampleRate);
    int samplesRendered = 0;

    int nextEvent = 0;

    while (samplesRendered < totalSamples) {
        audioBuffer.clear();
        midiBuffer.clear();

        double start = samplesRendered / sampleRate;
        double end   = (samplesRendered + blockSize) / sampleRate;

        while (nextEvent < fullSequence.getNumEvents()) {
            auto* ev = fullSequence.getEventPointer(nextEvent);
            double ts = ev->message.getTimeStamp();

            if (ts >= end)
                break;

            if (ts >= start) {
                int posInBlock = (int)((ts - start) * sampleRate);
                midiBuffer.addEvent(ev->message, posInBlock);
            }

            nextEvent++;
        }

        juce::AudioSourceChannelInfo info(&audioBuffer, 0, blockSize);
        juce::MidiBuffer mbCopy = midiBuffer;
        plugin->processBlock(audioBuffer, mbCopy);

        writer->writeFromAudioSampleBuffer(audioBuffer, 0, blockSize);

        samplesRendered += blockSize;
    }

    plugin->releaseResources();
}

