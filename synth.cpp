#include "synth.h"
#include "SynthParams.h"
#include "EventQueue.h"

Synth* Synth::instance_ = nullptr;


Synth::Synth(
    ParameterState& params,
    EvQueue& q,
    float sampleRate) :
    evq{q},
    params_{params},
    sampleRate_{sampleRate}
{
    osc.Init(sampleRate);
    osc.SetAmp(0);
}

void Synth::Process(float *sampleL, float *sampleR, size_t size) {
    Ev e;
    while(evq.pop(e)) {
        if (e.type == EV_NOTE_ON) {
            NoteOn(e.freq, e.vel);
        }
        else if (e.type == EV_NOTE_OFF) {
            NoteOff();
        }
    }

    SynthParams p = params_.GetReadParams();

    float target_amp = p.amplitude;
    amp_slew_ = (target_amp - current_amp_) / size;

    for (size_t i=0; i < size; i++) {
        current_amp_ += amp_slew_;
        sampleL[i] = sampleR[i] = current_amp_ * osc.Process();
    }
}

void Synth::NoteOn(float f, float amp) {
    osc.SetAmp(amp);
    osc.SetFreq(f);
}

void Synth::NoteOff() {
    osc.SetAmp(0);
}
