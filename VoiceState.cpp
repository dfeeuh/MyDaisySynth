
#include "VoiceState.h"
#include "MidiTables.h"


bool VoiceState::NoteOn(uint8_t note)
{
    for(int i = 0; i < count_; i++)
    {
        if(notes_[i] == note)
            return false;
    }

    if(count_ < kMaxHeldNotes)
        notes_[count_++] = note;

    uint8_t currentNote = notes_[count_ - 1];

    targetFrequency_ = kMidiNoteToFreq[currentNote];

    return true;
}

void VoiceState::NoteOff(uint8_t note)
{
    for(int i = 0; i < count_; i++)
    {
        if(notes_[i] == note)
        {
            // remove by shifting down
            for(int j = i; j < count_ - 1; ++j)
            {
                notes_[j] = notes_[j + 1];
            }
            --count_;
            break;
        }
    }

    if(count_ > 0)
    {
        uint8_t currentNote = notes_[count_ - 1];
        targetFrequency_    = kMidiNoteToFreq[currentNote];
    }
}
