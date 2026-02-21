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
    {
        daisy::ControlChangeEvent p = m.AsControlChange();
        if (p.control_number == 1) {
            onSetAmp(p.value);
        }
        break;
    }
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

void MidiManager::onSetAmp(uint8_t amp) {
    Ev event = { EV_PARAM};
    event.amp = (float) amp / 127;
    evq->push(event);
}
