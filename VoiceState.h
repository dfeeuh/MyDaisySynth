#ifndef VOICE_STATE_H
#define VOICE_STATE_H


struct VoiceState
{
    static constexpr int kMaxHeldNotes = 8;

    uint8_t notes_[kMaxHeldNotes];
    int     count_ = 0;

    float targetFrequency_;
    float currentFrequency_;

    /* Takes MIDI note and normalise velocity. Returns true if new note */
    bool NoteOn(uint8_t note);

    void NoteOff(uint8_t note);
    bool isEmpty() { return count_ == 0; }
};

#endif /* VOICE_STATE_H */
