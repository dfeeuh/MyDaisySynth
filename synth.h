
#include "daisy_seed.h"
#include "daisysp.h"

class ParameterState;
class EvQueue;

class SmoothedParam
{
  public:
    SmoothedParam(float smoothRate = 0.001f) : smoothRate_{smoothRate} {}

    void Set(float x) { target_ = x; }

    float Process()
    {
        current_ += smoothRate_ * (target_ - current_);
        return current_;
    }

  private:
    float target_;
    float current_;
    float smoothRate_;
};

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

    void NoteOn(float freq, float amp);
    void NoteOff();

  private:
    daisysp::Oscillator osc;
    daisysp::Adsr       env;

    // Shared references
    EvQueue&        evq;
    ParameterState& params_;

    static Synth* instance_;

    SmoothedParam masterAmplitude_;
    float         noteVelocity_;

    float sampleRate_;
    bool  env_gate_ = false;
};
