#include "synth.h"


std::unique_ptr<Synth> Synth::instance_ = nullptr;


void Synth::AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size) {
    for (size_t i=0; i < size; i++) {
        Synth::Instance()->Process(&out[1][i]);
        out[0][i] = out[1][i];
    }
}

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
