#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#define JUCE_MODULE_AVAILABLE_juce_core 1
#define JUCE_MODULE_AVAILABLE_juce_events 1
#define JUCE_MODULE_AVAILABLE_juce_audio_basics 1

#include "../external/JUCE/modules/juce_core/juce_core.h"
#include "../external/JUCE/modules/juce_graphics/juce_graphics.h"
#include "../external/JUCE/modules/juce_gui_basics/juce_gui_basics.h"
#include "../external/JUCE/modules/juce_audio_basics/juce_audio_basics.h"
#include "../external/JUCE/modules/juce_audio_processors/juce_audio_processors.h"
#include "../external/JUCE/modules/juce_graphics/juce_graphics.h"
#include "../external/JUCE/modules/juce_events/juce_events.h"

// An amount of samples from the beginning ot the track
// used to show time when sth should be played exactly;
using SampleTime = uint64_t;
