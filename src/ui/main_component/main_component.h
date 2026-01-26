#pragma once

#include "../../juce_header.h"
#include "../pianoroll/pianoroll.h"
#include "../instrumental_track_list/instrumental_track_list.h"
#include "../../audio_engine/track_handler/track_handler.h"
#include "../../queue/queue.h"
#include "../../queue/midi_command.h"
#include <memory>
#include <vector>
#include <chrono>

class MainComponent : public juce::Component {
public:
    MainComponent(std::shared_ptr<Queue<MidiCommand, 1024>>, std::unique_ptr<TrackHandler>);
    ~MainComponent() override;
    
    void paint(juce::Graphics&) override;
    void resized() override;
    
private:
    void closePianoRoll();
    void addInstrumentTrack();
    void showPianoRollForTrack(int trackIndex);
    void renderToWav();
    bool renderTracksSequential(std::vector<juce::File>& tempWavFiles, double& renderTime);
    bool renderTracksParallel(std::vector<juce::File>& tempWavFiles, double& renderTime);
    void mixWavFiles(const std::vector<juce::File>&, const juce::String&);
    
    std::shared_ptr<Queue<MidiCommand, 1024>> m_queue;
    std::unique_ptr<TrackHandler> m_trackHandler;
    
    PianoRoll m_pianoroll;
    InstrumentalTrackList m_trackList;
    juce::TextButton m_closePianoButton;
    juce::TextButton m_addTrackButton;
    juce::TextButton m_renderButton;
    juce::ComboBox m_renderModeCombo;  // Выбор режима рендеринга
    juce::TextEditor m_filenameEditor;
    juce::Label m_filenameLabel;
    juce::Label m_timeLabel;  // Для отображения времени
    
    bool m_pianorollVisible = false;
    int m_currentTrackIndex = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
