#ifndef SYNTH_PARAMS_H
#define SYNTH_PARAMS_H

#include "core_cm7.h" // or cm4 depending on Seed
#include "daisysp.h"


#include <array>

enum ParamID
{
    Osc1Amp,
    Osc2Amp,
    OscDetune,
    AmpAttack,
    AmpDecay,
    AmpSustain,
    AmpRelease,
    FilterCutoff,
    FilterReson,
    FilterAttack,
    FilterDecay,
    FilterSustain,
    FilterRelease,
    LfoFreq,
    LfoDepth,
    MasterAmplitude,
    Count
};

struct SynthParams
{
    SynthParams();

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
    SynthParams& GetWriteParams();
    void         Commit();
    SynthParams  GetReadParams() const;

  private:
    SynthParams      params_[2];
    volatile uint8_t write_index_ = 0;
};

#endif /* SYNTH_PARAMS_H */
