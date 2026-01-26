#include "juce_header.h"
#include "daw3/daw3.h"
#include "queue/queue.h"
#include "queue/midi_command.h"
#include <memory>

auto main() -> int {
    auto q = std::make_shared<Queue<MidiCommand, 1024>>();
    DAW3 daw3(q);
    daw3.start_audio_engine();
    daw3.init_ui(q);
    daw3.run_ui();
    return 0;
}
