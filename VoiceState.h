#ifndef VOICE_STATE_H
#define VOICE_STATE_H

#include "daisysp.h"

#include "SmoothedParam.h"

struct VoiceState
{
    VoiceState () : frequency_(0.1f) {}

    static constexpr int kMaxHeldNotes = 8;

    uint8_t notes_[kMaxHeldNotes];
    int     count_ = 0;

    SmoothedParam frequency_;

    /* Takes MIDI note and normalise velocity. Returns true if new note */
    bool NoteOn(uint8_t note);

    void NoteOff(uint8_t note);
    bool isEmpty() { return count_ == 0; }
};

#endif /* VOICE_STATE_H */
