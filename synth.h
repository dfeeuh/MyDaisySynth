
#include "daisy_seed.h"
#include "daisysp.h"

#include "EventQueue.h"

#include <memory>

using namespace daisy;
using namespace daisysp;

class Synth
{
public:
    Synth(std::shared_ptr<EvQueue> q, float sampleRate) :
        freq{1000},
        amp{0.25},
        evq{q}
    {
        osc.Init(sampleRate);
    }
    ~Synth() = default;

    // Single channel processing
    void Process(float* sample);

    void NoteOn(uint8_t note, uint8_t vel);
    void NoteOff(uint8_t note);

    void setFreq(float f);
    void setAmp(float a);

private:
    Oscillator osc;

    float freq;
    float amp;

    std::shared_ptr<EvQueue> evq;
};
