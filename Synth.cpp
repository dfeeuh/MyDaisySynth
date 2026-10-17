#include "Synth.h"
#include "SynthParams.h"
#include "EventQueue.h"
#include "VoiceState.h"

Synth* Synth::instance_ = nullptr;


Synth::Synth(ParameterState& params, EvQueue& q, float sampleRate)
: evq{q}, params_{params}, sampleRate_{sampleRate}, glide_{0.1f}
{
    osc1_.Init(sampleRate);
    osc1_.SetWaveform(daisysp::Oscillator::WAVE_SQUARE);
    osc1_.SetAmp(1.0f); // Set separately to master gain

    osc2_.Init(sampleRate);
    osc2_.SetWaveform(daisysp::Oscillator::WAVE_SAW);
    osc2_.SetAmp(1.0f);

    osc_detune_factor_.Set(0.0f);

    amp_env_.Init(sampleRate);
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

    float oscOffset = osc_detune_factor_.Process() * 0.5f;
    osc1_.SetFreq(currentFreq * (1.0f - oscOffset));
    osc2_.SetFreq(currentFreq * (1.0f + oscOffset));

    updateFromParams();

    for(size_t i = 0; i < size; i++)
    {
        filter_.SetFreq(filterCutoff_.Process());
        filter_.SetRes(filterReson_.Process());

        float env_out = amp_env_.Process(env_gate_);
        float osc1_out = osc1_.Process();
        float osc2_out = osc2_.Process();

        float gain = (1.f - lfo_depth_ * 0.5f)  // base drops as depth rises
                     + lfo_.Process();           // limits swing to ±0.5 max

        float mod_amp = gain * env_out;

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
        amp_env_.Retrigger(false);
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

    // ADSR Envelope
    amp_env_.SetAttackTime(p.values[ParamID::AmpAttack]);
    amp_env_.SetDecayTime(p.values[ParamID::AmpDecay]);
    amp_env_.SetSustainLevel(p.values[ParamID::AmpSustain]);
    amp_env_.SetReleaseTime(p.values[ParamID::AmpRelease]);

    // Update to the smoothed filter parameters
    filterCutoff_.Set(p.values[ParamID::FilterCutoff]);
    filterReson_.Set(p.values[ParamID::FilterReson]);

    lfo_.SetFreq(p.values[ParamID::LfoFreq]);
    lfo_depth_ = p.values[ParamID::LfoDepth];
    lfo_.SetAmp(lfo_depth_ * 0.5); // limits swing to ±0.5 max

    osc_detune_factor_.Set(p.values[ParamID::OscDetune]);

    masterAmplitude_.Set(p.values[ParamID::MasterAmplitude]);
}
