#pragma once
#include <memory>
#include "../queue/queue.h"
#include "../queue/midi_command.h"
#include "../audio_engine/audio_engine.h"
#include "../ui/ui.h"

/**
* @brief класс управляющий AudioEngine и UI, связывает их между собой и инициализирует.
*/
class DAW3 {
    public:
        DAW3(std::shared_ptr<Queue<MidiCommand, 1024>>);
        ~DAW3() = default;
        /**
        * @brief Инициализирует AudioEngine и AudioDeviceManager внутри него
        */
        void start_audio_engine();
        /**
        * @brief Инициализирует UI
        * @param LockFree очередь
        */
        void init_ui(std::shared_ptr<Queue<MidiCommand, 1024>>);
        /**
        * @brief Запускает UI (Dispatch Loop)
        */
        void run_ui();
        void add_track(InstrumentalTrack&&);
    private:
        std::unique_ptr<UI> m_ui;
        std::unique_ptr<AudioEngine> m_audio_engine;
};
