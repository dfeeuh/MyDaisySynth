#include "MidiManager.h"
#include "SynthParams.h"
#include "EventQueue.h"

#include "daisysp.h"


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
        onHandleCc(p.control_number, p.value);
        break;
    }
    default:
        return;
    }
}

void MidiManager::onNoteOn(uint8_t note, uint8_t vel) {
    float f = daisysp::mtof(note);
    float amp = (float) vel / 127;
    evq.push({EV_NOTE_ON, f, amp});
}

void MidiManager::onNoteOff(uint8_t note) {
    evq.push({EV_NOTE_OFF, 1.0, 0});
}

void MidiManager::onHandleCc(uint8_t cc, uint8_t value) {
    auto& p = params_.GetWriteParams();

    switch(cc)
    {
    case 11: // Master Amplitude
        p.amplitude = (float)value / 127;
        dirty_ = true;
    default:
        return;
    }
}

void MidiManager::CommitIfDirty() {
    if (dirty_) {
        params_.Commit();
        dirty_ = false;
    }
}
