#include "Synth.h"
#include "SynthParams.h"
#include "EventQueue.h"
#include "VoiceState.h"

Synth* Synth::instance_ = nullptr;


Synth::Synth(ParameterState& params, EvQueue& q, float sampleRate)
: base_amp_{1.0}, evq{q}, params_{params}, sampleRate_{sampleRate}, glide_{0.1f}
{
    osc1_.Init(sampleRate);
    osc1_.SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
    osc1_.SetAmp(1.0f); // Set separately to master gain

    osc2_.Init(sampleRate);
    osc2_.SetWaveform(daisysp::Oscillator::WAVE_SAW);
    osc2_.SetAmp(1.0f);

    osc2_freq_multiplier_.Set(1.01f);

    env_.Init(sampleRate);
    filter_.Init(sampleRate);

    lfo_.Init(sampleRate);
    lfo_.SetWaveform(daisysp::Oscillator::WAVE_SIN);

    updateFromParams();
}

void Synth::Process(float* sampleL, float* sampleR, size_t size)
{
    Ev e;
    while(evq.pop(e))
    {
        if(e.type == EV_NOTE_ON)
        {
            NoteOn(e.note, e.vel);
        }
        else if(e.type == EV_NOTE_OFF)
        {
            NoteOff(e.note);
        }
    }

    float currentFreq = voice_.frequency_.Process();

    osc1_.SetFreq(currentFreq);
    osc2_.SetFreq(currentFreq * osc2_freq_multiplier_.Process());

    updateFromParams();

    for(size_t i = 0; i < size; i++)
    {
        filter_.SetFreq(filterCutoff_.Process());
        filter_.SetRes(filterReson_.Process());

        float env_out = env_.Process(env_gate_);
        float osc1_out = osc1_.Process();
        float osc2_out = osc2_.Process();

        float mod_amp = (base_amp_ + lfo_.Process()) * env_out;

        float voice_amp = mod_amp * velocity_;
        float adsr_out  = 0.5 * (osc1_out + osc2_out) * voice_amp;

        float filt_out = filter_.Process(adsr_out);
        float final    = filt_out * masterAmplitude_.Process();

        sampleL[i] = final;
        sampleR[i] = final;
    }
}

void Synth::NoteOn(uint8_t note, float amp)
{
    bool wasEmpty = voice_.isEmpty();

    if(!voice_.NoteOn(note))
    {
        return; // Note already playing, do nothing
    }

    if(wasEmpty)
    {
        velocity_ = amp;
        env_gate_ = true;
        // Retrigger results in discontinuities.
        env_.Retrigger(false);
    }
    else
    {
        // legator - no retrigger
    }
}

void Synth::NoteOff(uint8_t note)
{
    voice_.NoteOff(note);
    if(voice_.isEmpty())
    {
        // No notes left
        env_gate_ = false;
    }
}

void Synth::updateFromParams()
{
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

    lfo_.SetFreq(p.values[ParamID::LfoFreq]);
    lfo_.SetAmp(p.values[ParamID::LfoDepth]);
}
