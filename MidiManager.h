#pragma once

#include "daisy_seed.h"


class ParameterState;
class EvQueue;

class MidiManager
{
public:
    MidiManager(ParameterState& params, EvQueue& q);

    ~MidiManager() = default;

    void HandleMidiMessage(daisy::MidiEvent m);

    void CommitIfDirty();

private:
    void onNoteOn(uint8_t note, uint8_t vel);
    void onNoteOff(uint8_t note);

    void onHandleCc(uint8_t cc, uint8_t value);

    void initCcMap();

    ParameterState& params_;
    EvQueue& evq;

    bool dirty_ = false;
};
