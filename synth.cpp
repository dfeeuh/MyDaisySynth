#include "synth.h"
#include "SynthParams.h"
#include "EventQueue.h"

Synth* Synth::instance_ = nullptr;


Synth::Synth(ParameterState& params, EvQueue& q, float sampleRate)
: evq{q}, params_{params}, sampleRate_{sampleRate}
{
    osc.Init(sampleRate);
    osc.SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
    osc.SetAmp(1.0f); // Set separately to master gain
    env.Init(sampleRate);
}

void Synth::Process(float* sampleL, float* sampleR, size_t size)
{
    Ev e;
    while(evq.pop(e))
    {
        if(e.type == EV_NOTE_ON)
        {
            NoteOn(e.freq, e.vel);
        }
        else if(e.type == EV_NOTE_OFF)
        {
            NoteOff();
        }
    }

    SynthParams p = params_.GetReadParams();

    masterAmplitude_.Set(p.values[ParamID::MasterAmplitude]);

    env.SetAttackTime(p.values[ParamID::Attack]);
    env.SetDecayTime(p.values[ParamID::Decay]);
    env.SetSustainLevel(p.values[ParamID::Sustain]);
    env.SetReleaseTime(p.values[ParamID::Release]);

    for(size_t i = 0; i < size; i++)
    {
        float env_out = env.Process(env_gate_);
        float osc_out = osc.Process();

        float voice_amp = env_out * noteVelocity_;
        float final     = osc_out * voice_amp * masterAmplitude_.Process();

        sampleL[i] = final;
        sampleR[i] = final;
    }
}

void Synth::NoteOn(float f, float amp)
{
    osc.SetFreq(f);
    noteVelocity_ = amp;
    env_gate_     = true;

    // Retrigger results in discontinuities.
    env.Retrigger(false);
}

void Synth::NoteOff()
{
    env_gate_ = false;
}
