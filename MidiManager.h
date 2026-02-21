#pragma once

#include "daisy_seed.h"
#include "EventQueue.h"

#include <memory>


class MidiManager
{
public:
    MidiManager(std::shared_ptr<EvQueue> q) : evq{q} {}
    ~MidiManager() = default;

    void HandleMidiMessage(daisy::MidiEvent m);

private:
    void onNoteOn(uint8_t note, uint8_t vel);
    void onNoteOff(uint8_t note);

    std::shared_ptr<EvQueue> evq;
};
