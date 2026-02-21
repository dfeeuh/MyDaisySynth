#include "synth.h"

void Synth::Process(float* sample) {
    Ev e;
    while(evq->pop(e)) {
        if (e.type == EV_NOTE_ON) {
            NoteOn(e.note, e.vel);
        }
        else if (e.type == EV_NOTE_OFF) {
            NoteOff(e.note);
        }
    }

    *sample = osc.Process();
}

void Synth::setFreq(float f) {
    freq = f;
    osc.SetFreq(freq);
}

void Synth::setAmp(float a) {
    amp = a;
}

void Synth::NoteOn(uint8_t note, uint8_t vel) {
    float amp = (float)vel / 127;
    osc.SetAmp(amp);
    osc.SetFreq(mtof(note));
}

void Synth::NoteOff(uint8_t note) {
    osc.SetAmp(0);
}
