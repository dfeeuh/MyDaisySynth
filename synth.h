
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

class Synth
{
public:
    Synth(const float sampleRate) : freq{1000}, amp{0.25} {
        osc.Init(sampleRate);
    }
    ~Synth() = default;

    // Single channel processing
    void Process(float* sample);

    void setFreq(float f);
    void setAmp(float a);

private:
    Oscillator osc;

    float freq;
    float amp;
};
