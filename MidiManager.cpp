#include "MidiManager.h"

void MidiManager::HandleMidiMessage(daisy::MidiEvent m) {

    switch(m.type)
    {
    case daisy::NoteOn:
    {
        daisy::NoteOnEvent p = m.AsNoteOn();
        onNoteOn(p.note, p.velocity);
        break;
    }
    case daisy::NoteOff:
    {
        daisy::NoteOnEvent p = m.AsNoteOn();
        onNoteOff(p.note);
        break;
    }
    case daisy::ControlChange:
        break;
    default:
        return;
    }
}

void MidiManager::onNoteOn(uint8_t note, uint8_t vel) {
    evq->push({EV_NOTE_ON, note, vel});
}

void MidiManager::onNoteOff(uint8_t note) {
    evq->push({EV_NOTE_OFF, note, 0});
}

