#include "MidiManager.h"
#include "SynthParams.h"
#include "EventQueue.h"

#include "daisysp.h"

inline float MapLogTime(uint8_t value,
                        float min_time,
                        float max_time)
{
    float t = value / 127.f;          // normalize 0–1
    return min_time * powf(max_time / min_time, t);
}


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
    float amp = vel / 127.f;
    evq.push({EV_NOTE_ON, f, amp});
}

void MidiManager::onNoteOff(uint8_t note) {
    evq.push({EV_NOTE_OFF});
}

void MidiManager::onHandleCc(uint8_t cc, uint8_t value) {
    auto& p = params_.GetWriteParams();

    switch(cc)
    {
    case 11: // Master Amplitude
        p.amplitude = value / 127.f;
        dirty_ = true;
        break;

    case 12: // Attack
        p.attack = MapLogTime(value, 0.001f, 10.0f);
        dirty_ = true;
        break;

    case 13: // Decay
        p.decay = MapLogTime(value, 0.005f, 10.0f);
        dirty_ = true;
        break;

    case 14: // Sustain
        p.sustain = value / 127.f;   // sustain is level, linear is correct
        dirty_ = true;
        break;

    case 15: // Release
        p.release = MapLogTime(value, 0.005f, 15.0f);
        dirty_ = true;
        break;

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
