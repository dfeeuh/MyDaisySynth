#include "synth.h"
#include "SynthParams.h"
#include "EventQueue.h"

Synth* Synth::instance_ = nullptr;


Synth::Synth(ParameterState& params, EvQueue& q, float sampleRate)
: evq{q}, params_{params}, sampleRate_{sampleRate}
{
    osc_.Init(sampleRate);
    osc_.SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
    osc_.SetAmp(1.0f); // Set separately to master gain
    env_.Init(sampleRate);

    filter_.Init(sampleRate);
    filter_.SetFreq(sampleRate / 2.f);
    filter_.SetRes(0);
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

    // ADSR Envelope
    env_.SetAttackTime(p.values[ParamID::Attack]);
    env_.SetDecayTime(p.values[ParamID::Decay]);
    env_.SetSustainLevel(p.values[ParamID::Sustain]);
    env_.SetReleaseTime(p.values[ParamID::Release]);

    // Update to the smoothed filter parameters
    filterCutoff_.Set(p.values[ParamID::FilterCutoff]);
    filterReson_.Set(p.values[ParamID::FilterReson]);

    for(size_t i = 0; i < size; i++)
    {
        filter_.SetFreq(filterCutoff_.Process());
        filter_.SetRes(filterReson_.Process());

        float env_out = env_.Process(env_gate_);
        float osc_out = osc_.Process();

        float voice_amp = env_out * noteVelocity_;
        float adsr_out  = osc_out * voice_amp;

        float filt_out = filter_.Process(adsr_out);
        float final    = filt_out * masterAmplitude_.Process();

        sampleL[i] = final;
        sampleR[i] = final;
    }
}

void Synth::NoteOn(float f, float amp)
{
    osc_.SetFreq(f);
    noteVelocity_ = amp;
    env_gate_     = true;

    // Retrigger results in discontinuities.
    env_.Retrigger(false);
}

void Synth::NoteOff()
{
    env_gate_ = false;
}
