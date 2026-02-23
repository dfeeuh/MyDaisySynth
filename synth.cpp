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
    //osc.SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
    osc.SetAmp(1.0f); // Set separately to master gain
    env.Init(sampleRate);
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

    float vel_slew = (note_velocity_target_ - note_velocity_) / size;

    SynthParams p = params_.GetReadParams();

    // Master volume - slew to smooth (reduce zipper)
    float amp_slew = (p.amplitude - current_amp_) / size;

    env.SetAttackTime(p.attack);
    env.SetDecayTime(p.decay);
    env.SetSustainLevel(p.sustain);
    env.SetReleaseTime(p.release);

    for (size_t i=0; i < size; i++) {
        current_amp_ += amp_slew;
        note_velocity_ += vel_slew;

        float env_level = env.Process(env_gate_);

        float s = osc.Process();

        float final = s * env_level * note_velocity_ * current_amp_;

        sampleL[i] = final;
        sampleR[i] = final;
    }
}

void Synth::NoteOn(float f, float amp) {
    osc.SetFreq(f);
    note_velocity_target_ = amp;
    env_gate_ = true;

    // Retrigger results in discontinuities.
    env.Retrigger(false);
}

void Synth::NoteOff() {
    env_gate_ = false;
}
