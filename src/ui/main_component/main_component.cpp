#include "main_component.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>

#include "main_component.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include <memory>
#include <thread>
#include <future>
#include <vector>
#include <chrono>

MainComponent::MainComponent(std::shared_ptr<Queue<MidiCommand, 1024>> q, std::unique_ptr<TrackHandler> th)
    : m_queue(std::move(q)), 
      m_pianoroll(q, 0),
      m_closePianoButton("X"),
      m_addTrackButton("+ Add VST"),
      m_renderButton("Render to WAV"),
      m_filenameLabel("Filename:", "Output filename:"),
      m_timeLabel("", "Render time will be shown here"),
      m_trackHandler(std::move(th)) {
    
    m_closePianoButton.onClick = [this] { closePianoRoll(); };
    m_addTrackButton.onClick = [this] { addInstrumentTrack(); };
    m_renderButton.onClick = [this] { renderToWav(); };
    
    m_renderModeCombo.addItem("Sequential Rendering", 1);
    m_renderModeCombo.addItem("Parallel Rendering", 2);
    m_renderModeCombo.setSelectedId(1);
    
    m_filenameEditor.setText("output");
    m_filenameEditor.setMultiLine(false);
    m_filenameEditor.setReturnKeyStartsNewLine(false);
    
    m_pianoroll.setVisible(false);
    m_pianoroll.setEnabled(false);
    m_closePianoButton.setVisible(false);
    m_renderButton.setVisible(true);
    m_renderModeCombo.setVisible(true);
    m_filenameEditor.setVisible(true);
    m_filenameLabel.setVisible(true);
    m_timeLabel.setVisible(true);
    
    m_trackList.onTrackSelected = [this](int index) {
        showPianoRollForTrack(index);
    };
    
    addAndMakeVisible(m_closePianoButton);
    addAndMakeVisible(m_addTrackButton);
    addAndMakeVisible(m_renderButton);
    addAndMakeVisible(m_renderModeCombo);
    addAndMakeVisible(m_filenameLabel);
    addAndMakeVisible(m_filenameEditor);
    addAndMakeVisible(m_timeLabel);
    addAndMakeVisible(m_trackList);
    addChildComponent(m_pianoroll);
    
    setSize(1200, 800);
}

MainComponent::~MainComponent() {
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

void MainComponent::resized() {
    auto bounds = getLocalBounds();
    
    auto topPanel = bounds.removeFromTop(40).reduced(5);
    
    if (m_pianorollVisible) {
        m_closePianoButton.setVisible(true);
        m_addTrackButton.setVisible(true);
        m_renderButton.setVisible(false);
        m_renderModeCombo.setVisible(false);
        m_filenameLabel.setVisible(false);
        m_filenameEditor.setVisible(false);
        m_timeLabel.setVisible(false);
        
        m_closePianoButton.setBounds(topPanel.removeFromLeft(40));
        topPanel.removeFromLeft(10);
        m_addTrackButton.setBounds(topPanel.removeFromLeft(120));
        
        m_pianoroll.setBounds(bounds);
        m_pianoroll.setVisible(true);
        m_trackList.setVisible(false);
    } else {
        m_closePianoButton.setVisible(false);
        m_addTrackButton.setVisible(true);
        m_renderButton.setVisible(true);
        m_renderModeCombo.setVisible(true);
        m_filenameLabel.setVisible(true);
        m_filenameEditor.setVisible(true);
        m_timeLabel.setVisible(true);
        
        auto buttonArea = topPanel.removeFromLeft(800);
        m_addTrackButton.setBounds(buttonArea.removeFromLeft(120));
        buttonArea.removeFromLeft(10);
        m_renderButton.setBounds(buttonArea.removeFromLeft(150));
        buttonArea.removeFromLeft(10);
        m_renderModeCombo.setBounds(buttonArea.removeFromLeft(150).reduced(2));
        buttonArea.removeFromLeft(10);
        m_filenameLabel.setBounds(buttonArea.removeFromLeft(80));
        m_filenameEditor.setBounds(buttonArea.removeFromLeft(200).reduced(2));
        
        auto timePanel = bounds.removeFromTop(25).reduced(5);
        m_timeLabel.setBounds(timePanel);
        
        m_trackList.setBounds(bounds);
        m_trackList.setVisible(true);
        m_pianoroll.setVisible(false);
    }
}

void MainComponent::closePianoRoll() {
    m_pianorollVisible = false;
    m_pianoroll.setVisible(false);
    m_pianoroll.setEnabled(false);
    m_trackList.clearSelection();
    resized();
}

void MainComponent::showPianoRollForTrack(int trackIndex) {
    m_currentTrackIndex = trackIndex;
    m_pianorollVisible = true;
    m_pianoroll.setCurrentTrackID(trackIndex);
    m_pianoroll.setVisible(true);
    m_pianoroll.setEnabled(true);
    resized();
}

void MainComponent::addInstrumentTrack() {
    auto flags = juce::FileBrowserComponent::openMode | 
                 juce::FileBrowserComponent::canSelectFiles |
                 juce::FileBrowserComponent::canSelectDirectories;
    
    auto chooser = std::make_shared<juce::FileChooser>(
        "Select VST Plugin",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*.vst;*.vst3;*.component;*.dll;*.so"
    );
    
    chooser->launchAsync(flags, [this, chooser](const juce::FileChooser& fc) {
        auto result = fc.getResult();
        if (result.exists()) {
            auto plugin = InstrumentalTrack::load_plugin(result);
            if (plugin) {
                auto trackName = result.getFileNameWithoutExtension();
                auto track = std::make_shared<InstrumentalTrack>(
                    trackName.toStdString(),
                    std::move(plugin)
                );
                m_trackHandler.get()->addTrack(track);
                m_trackList.addTrack(track);
            }
        }
    });
}

void MainComponent::renderToWav() {
    if (m_trackHandler->getNumTracks() == 0) {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
            "No tracks",
            "Add at least one instrument track before rendering.",
            "OK");
        return;
    }
    
    auto fileChooserFlags = juce::FileBrowserComponent::saveMode |
                            juce::FileBrowserComponent::canSelectFiles |
                            juce::FileBrowserComponent::warnAboutOverwriting;
    
    auto chooser = std::make_shared<juce::FileChooser>(
        "Save WAV file",
        juce::File::getCurrentWorkingDirectory().getChildFile(m_filenameEditor.getText()),
        "*.wav",
        true
    );
    
    chooser->launchAsync(fileChooserFlags, [this, chooser](const juce::FileChooser& fc) {
        auto result = fc.getResult();
        if (result.getFullPathName().isEmpty()) {
            return;
        }
        
        juce::String outputPath = result.getFullPathName();
        if (!outputPath.endsWithIgnoreCase(".wav")) {
            outputPath += ".wav";
        }
        
        std::vector<juce::File> tempWavFiles;
        double renderTime = 0.0;
        bool success = false;
        
        int renderMode = m_renderModeCombo.getSelectedId();
        
        if (renderMode == 1) {
            success = renderTracksSequential(tempWavFiles, renderTime);
        } else if (renderMode == 2) {
            success = renderTracksParallel(tempWavFiles, renderTime);
        }
        
        if (!success || tempWavFiles.empty()) {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                "No audio",
                "No tracks were rendered.",
                "OK");
            return;
        }
        
        auto mixStart = std::chrono::high_resolution_clock::now();
        
        mixWavFiles(tempWavFiles, outputPath);
        
        auto mixEnd = std::chrono::high_resolution_clock::now();
        double mixTime = std::chrono::duration<double>(mixEnd - mixStart).count();
        
        for (auto& tempFile : tempWavFiles) {
            tempFile.deleteFile();
        }
        
        // Обновляем метку со временем
        juce::String modeStr = (renderMode == 1) ? "Sequential" : "Parallel";
        juce::String timeStr = juce::String::formatted(
            "%s render: %.2fs, Mix: %.2fs, Total: %.2fs",
            modeStr.toRawUTF8(),
            renderTime,
            mixTime,
            renderTime + mixTime
        );
        
        m_timeLabel.setText(timeStr, juce::dontSendNotification);
        
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
            "Render complete",
            juce::String(tempWavFiles.size()) + " tracks rendered (" + modeStr + ")\n" +
            "Render time: " + juce::String(renderTime, 2) + "s\n" +
            "Mix time: " + juce::String(mixTime, 2) + "s\n" +
            "Total time: " + juce::String(renderTime + mixTime, 2) + "s\n" +
            "Output: " + outputPath,
            "OK");
    });
}

bool MainComponent::renderTracksSequential(std::vector<juce::File>& tempWavFiles, double& renderTime) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int trackIndex = 0; trackIndex < m_trackHandler->getNumTracks(); ++trackIndex) {
        auto* track = m_trackHandler->getTrack(trackIndex);
        auto* pianoRollData = m_pianoroll.getTrackData(trackIndex);
        
        if (!track || !pianoRollData) continue;
        
        juce::String trackFileName = juce::File::getSpecialLocation(juce::File::tempDirectory)
            .getChildFile("temp_track" + juce::String(trackIndex) + ".wav").getFullPathName();
        
        auto midiSequence = pianoRollData->toMidiSequence(120.0);
        
        if (midiSequence.getNumEvents() == 0) continue;
        
        try {
            track->renderSequenceIntoWav(midiSequence, trackFileName.toStdString());
            tempWavFiles.push_back(juce::File(trackFileName));
            
        } catch (const std::exception& e) {
            juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                "Render error",
                "Failed to render track " + juce::String(trackIndex + 1) + ": " + e.what(),
                "OK");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    renderTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return !tempWavFiles.empty();
}

bool MainComponent::renderTracksParallel(std::vector<juce::File>& tempWavFiles, double& renderTime) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int totalTracks = m_trackHandler->getNumTracks();
    
    std::vector<std::thread> threads;
    std::vector<std::pair<bool, juce::String>> results(totalTracks);
    
    for (int trackIndex = 0; trackIndex < totalTracks; ++trackIndex) {
        auto* track = m_trackHandler->getTrack(trackIndex);
        auto* pianoRollData = m_pianoroll.getTrackData(trackIndex);
        
        if (!track || !pianoRollData) {
            results[trackIndex] = {false, ""};
            continue;
        }
        
        auto midiSequence = pianoRollData->toMidiSequence(120.0);
        if (midiSequence.getNumEvents() == 0) {
            results[trackIndex] = {false, ""};
            continue;
        }
        
        juce::String trackFileName = juce::File::getSpecialLocation(juce::File::tempDirectory)
            .getChildFile("parallel_track" + juce::String(trackIndex) + ".wav").getFullPathName();
        
        threads.emplace_back([track, midiSequence, trackFileName, trackIndex, &results]() {
            try {
                track->renderSequenceIntoWav(midiSequence, trackFileName.toStdString());
                results[trackIndex] = {true, trackFileName};
            } catch (const std::exception& e) {
                juce::Logger::writeToLog("Failed track " + juce::String(trackIndex + 1) + ": " + e.what());
                results[trackIndex] = {false, ""};
            }
        });
    }
    
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    for (auto& [success, fileName] : results) {
        if (success && !fileName.isEmpty()) {
            tempWavFiles.push_back(juce::File(fileName));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    renderTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return !tempWavFiles.empty();
}

void MainComponent::mixWavFiles(const std::vector<juce::File>& inputFiles, const juce::String& outputPath) {
    if (inputFiles.empty()) return;
    
    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatReader> firstReader(wavFormat.createReaderFor(
        new juce::FileInputStream(inputFiles[0]), true));
    
    if (!firstReader) return;
    
    double sampleRate = firstReader->sampleRate;
    int numChannels = firstReader->numChannels;
    juce::int64 numSamples = firstReader->lengthInSamples;
    int bitsPerSample = firstReader->bitsPerSample;
    
    juce::File outputFile(outputPath);
    outputFile.deleteFile();
    
    std::unique_ptr<juce::FileOutputStream> outputStream(outputFile.createOutputStream());
    if (!outputStream) return;
    
    std::unique_ptr<juce::AudioFormatWriter> writer(wavFormat.createWriterFor(
        outputStream.release(), sampleRate, numChannels, bitsPerSample, {}, 0));
    
    if (!writer) return;
    
    juce::AudioBuffer<float> mixBuffer(numChannels, (int)numSamples);
    mixBuffer.clear();
    
    juce::AudioBuffer<float> tempBuffer(numChannels, 4096);
    
    for (const auto& inputFile : inputFiles) {
        std::unique_ptr<juce::AudioFormatReader> reader(wavFormat.createReaderFor(
            new juce::FileInputStream(inputFile), true));
        
        if (!reader) continue;
        
        juce::int64 samplesRemaining = numSamples;
        juce::int64 samplePos = 0;
        
        while (samplesRemaining > 0) {
            int samplesToRead = juce::jmin(4096, (int)samplesRemaining);
            reader->read(&tempBuffer, 0, samplesToRead, (int)samplePos, true, true);
            
            for (int ch = 0; ch < numChannels; ++ch) {
                mixBuffer.addFrom(ch, (int)samplePos, tempBuffer, ch, 0, samplesToRead);
            }
            
            samplePos += samplesToRead;
            samplesRemaining -= samplesToRead;
        }
    }
    
    float maxLevel = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch) {
        maxLevel = juce::jmax(maxLevel, mixBuffer.getMagnitude(ch, 0, mixBuffer.getNumSamples()));
    }
    
    if (maxLevel > 1.0f) {
        mixBuffer.applyGain(1.0f / maxLevel);
    }
    
    writer->writeFromAudioSampleBuffer(mixBuffer, 0, mixBuffer.getNumSamples());
}
