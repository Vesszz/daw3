//#include "instrumental_track/instrumental_track.h"
#include "juce_header.h"
#include "daw3/daw3.h"
#include "instrumental_track/instrumental_track.h"
#include "track_handler/track_handler.h"

#define TEST_VST_PATH "/usr/lib/vst3/Odin2.vst3"
#define TEST_VST_PATH2 "/usr/lib/vst3/Spline.vst3"
#define TEST_MIDI_PATH "Bohemian-Rhapsody-1.mid"
#define TEST_WAV_PATH "here.wav"

auto main() -> int {
    DAW3 daw3 = {};
    daw3.getAudioDeviceManager().playTestSound();
    std::unique_ptr<juce::AudioPluginInstance> plugin = InstrumentalTrack::load_plugin(juce::File(TEST_VST_PATH), 44100.0f, 512);
    InstrumentalTrack it("name", std::move(plugin));
    it.render_midifile_into_wav(TEST_MIDI_PATH, TEST_WAV_PATH); 
    return 0;
}
