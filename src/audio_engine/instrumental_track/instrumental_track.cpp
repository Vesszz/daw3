#include "instrumental_track.h"
#include <stdexcept>

InstrumentalTrack::InstrumentalTrack(std::string name, std::unique_ptr<juce::AudioPluginInstance> plugin)
    : m_name(std::move(name)), m_audio_plugin_instance(std::move(plugin)) {
}

InstrumentalTrack::~InstrumentalTrack() {
    releaseResources();
}

const juce::String InstrumentalTrack::info() {
    return m_audio_plugin_instance ? m_audio_plugin_instance->getName() : "No plugin";
}

void InstrumentalTrack::prepareToPlay(double sampleRate, int blockSize) {
    m_sampleRate = sampleRate;
    m_blockSize = blockSize;
    
    if (m_audio_plugin_instance) {
        m_audio_plugin_instance->prepareToPlay(sampleRate, blockSize);
    }
}

void InstrumentalTrack::releaseResources() {
    if (m_audio_plugin_instance) {
        m_audio_plugin_instance->releaseResources();
    }
}

void InstrumentalTrack::renderAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiBuffer) {
    if (!m_audio_plugin_instance) {
        buffer.clear();
        return;
    }
    
    juce::MidiBuffer processedMidi = midiBuffer;
    m_audio_plugin_instance->processBlock(buffer, processedMidi);
}

void InstrumentalTrack::addMidiMessage(const juce::MidiMessage& msg, double timestamp) {
    m_midi_message_sequence.addEvent(msg, timestamp);
}

std::unique_ptr<juce::AudioPluginInstance> InstrumentalTrack::load_plugin(juce::File filename, float sampleRate, int blockSize) {
    juce::AudioPluginFormatManager formatManager;
    formatManager.addDefaultFormats();

    juce::OwnedArray<juce::PluginDescription> foundTypes;

    for (auto* fmt : formatManager.getFormats()) {
        if (fmt == nullptr) continue;
        fmt->findAllTypesForFile(foundTypes, filename.getFullPathName());
        if (foundTypes.size() > 0)
            break;
    }
    
    if (foundTypes.size() == 0) {
        return nullptr;
    }

    juce::String errorMessage;
    std::unique_ptr<juce::AudioPluginInstance> inst = formatManager.createPluginInstance(*foundTypes[0], sampleRate, blockSize, errorMessage);
    if (!inst) {
        return nullptr;
    }
    return inst;
}

static juce::MidiMessageSequence loadMidi(const std::string& path) {
    juce::File file(path);
    juce::FileInputStream stream(file);
    if (!stream.openedOk()) throw std::runtime_error("Failed to open MIDI file");

    juce::MidiFile mf;
    if (!mf.readFrom(stream)) throw std::runtime_error("Failed to read MIDI file");

    mf.convertTimestampTicksToSeconds();

    juce::MidiMessageSequence seq;
    for (int i = 0; i < mf.getNumTracks(); ++i)
        seq.addSequence(*mf.getTrack(i), 0.0);

    seq.updateMatchedPairs();
    return seq;
}

static std::unique_ptr<juce::AudioFormatWriter> createWriter(const std::string& path, double sampleRate, int channels) {
    juce::File file(path);
    file.deleteFile();

    juce::WavAudioFormat format;
    auto stream = file.createOutputStream();
    if (!stream) throw std::runtime_error("Failed to create output stream");

    auto writer = std::unique_ptr<juce::AudioFormatWriter>(format.createWriterFor(stream.release(), sampleRate, (unsigned int)channels, 16, {}, 0));

    if (!writer) throw std::runtime_error("Failed to create WAV writer");
    return writer;
}

static void preparePlugin(juce::AudioPluginInstance* plugin, double sampleRate, int blockSize) {
    if (!plugin) throw std::runtime_error("Audio plugin is null");
    plugin->prepareToPlay(sampleRate, blockSize);
}

static void renderBlock(juce::AudioPluginInstance* plugin, juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) {
    juce::MidiBuffer copy = midi;
    plugin->processBlock(audio, copy);
}

static void renderSequence(juce::AudioPluginInstance* plugin, juce::MidiMessageSequence& seq, juce::AudioFormatWriter& writer, double sampleRate, int blockSize) {
    int channels = plugin->getTotalNumOutputChannels();
    juce::AudioBuffer<float> audio(channels, blockSize);
    juce::MidiBuffer midi;

    int totalSamples = (int)(seq.getEndTime() * sampleRate);
    int rendered = 0;
    int nextEvent = 0;

    while (rendered < totalSamples) {
        audio.clear();
        midi.clear();

        double start = rendered / sampleRate;
        double end   = (rendered + blockSize) / sampleRate;

        while (nextEvent < seq.getNumEvents()) {
            auto* ev = seq.getEventPointer(nextEvent);
            double ts = ev->message.getTimeStamp();
            if (ts >= end) break;
            if (ts >= start)
                midi.addEvent(ev->message, (int)((ts - start) * sampleRate));
            ++nextEvent;
        }

        renderBlock(plugin, audio, midi);
        writer.writeFromAudioSampleBuffer(audio, 0, blockSize);
        rendered += blockSize;
    }
}

void InstrumentalTrack::renderMidifileIntoWav(std::string midiPath, std::string wavPath) {
    auto sequence = loadMidi(midiPath);

    auto* plugin = m_audio_plugin_instance.get();
    double sampleRate = 44100.0;
    int blockSize = 512;

    preparePlugin(plugin, sampleRate, blockSize);

    auto writer = createWriter(wavPath, sampleRate, plugin->getTotalNumOutputChannels());

    renderSequence(plugin, sequence, *writer, sampleRate, blockSize);
    plugin->releaseResources();
}

void InstrumentalTrack::renderSequenceIntoWav(juce::MidiMessageSequence sequence, std::string wavPath) {
    auto* plugin = m_audio_plugin_instance.get();
    double sampleRate = 44100.0;
    int blockSize = 512;

    preparePlugin(plugin, sampleRate, blockSize);

    auto writer = createWriter(wavPath, sampleRate, plugin->getTotalNumOutputChannels());

    renderSequence(plugin, sequence, *writer, sampleRate, blockSize);
    plugin->releaseResources();
}
