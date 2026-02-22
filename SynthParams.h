#include "core_cm7.h"  // or cm4 depending on Seed

struct SynthParams
{
    float amplitude;
    float cutoff;
    float resonance;
    float attack;
    float decay;
    float sustain;
    float release;
    // add as many as you like

    SynthParams() :
      amplitude{0.99f}
    {}
};

class ParameterState
{
public:
    SynthParams& GetWriteParams()
    {
        return params_[write_index_];
    }

    void Commit()
    {
        __DMB();
        write_index_ ^= 1;
    }

    SynthParams GetReadParams() const
    {
        uint8_t read = write_index_ ^ 1;
        return params_[read];  // snapshot
    }

private:
    SynthParams params_[2];
    volatile uint8_t write_index_ = 0;
};
