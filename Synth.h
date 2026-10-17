#ifndef SNYTH_H
#define SYNTH_H

#include "daisy_seed.h"
#include "daisysp.h"

#include "VoiceState.h"
#include "SmoothedParam.h"

class ParameterState;
class EvQueue;


class Synth
{
  public:
    static Synth* Create(ParameterState& params, EvQueue& q, float sampleRate)
    {
        if(!instance_)
        {
            static Synth synth(params, q, sampleRate);
            instance_ = &synth;
        }

        return instance_;
    }

    static Synth& Instance() { return *instance_; }

    Synth(ParameterState& params, EvQueue& q, float sampleRate);

    ~Synth() = default;

    void Process(float* sampleL, float* sampleR, size_t size);

    void NoteOn(uint8_t note, float amp);
    void NoteOff(uint8_t note);

  private:
    void updateFromParams(); // Control rate paramater update

    daisysp::Oscillator osc1_;
    daisysp::Oscillator osc2_;
    daisysp::Adsr       amp_env_;

    daisysp::Oscillator lfo_;
    float               lfo_depth_;  /* total oscillator amp plus LFO depth */

    // Shared references
    EvQueue&        evq;
    ParameterState& params_;

    static Synth* instance_;

    daisysp::MoogLadder filter_;
    SmoothedParam       filterCutoff_;
    SmoothedParam       filterReson_;
    SmoothedParam       osc_detune_factor_; /* DETUNE! multiplies osc2 freq  */

    SmoothedParam       masterAmplitude_;
    VoiceState voice_;
    float      velocity_;

    float sampleRate_;
    float glide_;
    bool  env_gate_ = false;
};

#endif /* SYNTH_H */
