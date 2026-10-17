#include "SynthParams.h"

SynthParams::SynthParams()
{
    // Default initialisation.
    values[AmpAttack]       = 0.005f;
    values[AmpDecay]        = 0.005f;
    values[AmpSustain]      = 1.0f;
    values[AmpRelease]      = 0.005f;
    values[FilterCutoff]    = 10000.f;
    values[FilterReson]     = 0.0f;
    values[LfoFreq]         = 0.0f;
    values[LfoDepth]        = 0.0f;
    values[OscDetune]       = 0.0f;
    values[MasterAmplitude] = 0.5f;
}

SynthParams& ParameterState::GetWriteParams()
{
    return params_[write_index_];
}

void ParameterState::Commit()
{
    __DMB();
    write_index_ ^= 1;
}

SynthParams ParameterState::GetReadParams() const
{
    uint8_t read = write_index_ ^ 1;
    return params_[read]; // snapshot
}
