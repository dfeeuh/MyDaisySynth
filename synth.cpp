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
    osc.SetAmp(1.0f);

    note_smoothing_coeff_ = 1.0f / (sampleRate_ * 5e-3f);
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
        note_gain_   += note_smoothing_coeff_ * (note_target_gain_ - note_gain_);

        float s = osc.Process();
        float final = s * note_gain_ * current_amp_;

        sampleL[i] = final;
        sampleR[i] = final;
    }
}

void Synth::NoteOn(float f, float amp) {
    note_target_gain_ = amp;
    osc.SetFreq(f);
}

void Synth::NoteOff() {
    note_target_gain_ = 0;
}
