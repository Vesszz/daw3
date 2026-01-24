//#include "instrumental_track/instrumental_track.h"
#include "juce_header.h"
#include "daw3/daw3.h"

auto main() -> int {
    DAW3 daw3 = {};
    //daw3.getAudioDeviceManager().playTestSound();
    daw3.init_ui();
    daw3.run_ui();
    return 0;
}
