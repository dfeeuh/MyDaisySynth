#include "core_cm7.h" // or cm4 depending on Seed
#include "daisysp.h"


#include <array>

enum ParamID
{
    MasterAmplitude = 0,
    Attack,
    Decay,
    Sustain,
    Release,
    FilterCutoff,
    FilterReson,
    Count
};

struct SynthParams
{
    SynthParams() {
        values[MasterAmplitude] = 0.5f;
        values[FilterCutoff] = 10000.0f;
        values[FilterReson] = 0.0f;
    }

    float values[ParamID::Count];
};

struct ParamDescriptor
{
    float            min;
    float            max;
    daisysp::Mapping mapping;
};


class ParameterState
{
  public:
    SynthParams& GetWriteParams() { return params_[write_index_]; }

    void Commit()
    {
        __DMB();
        write_index_ ^= 1;
    }

    SynthParams GetReadParams() const
    {
        uint8_t read = write_index_ ^ 1;
        return params_[read]; // snapshot
    }

  private:
    SynthParams      params_[2];
    volatile uint8_t write_index_ = 0;
};
