
#include "daisy_seed.h"
#include "daisysp.h"

#include "EventQueue.h"

#include <memory>

using namespace daisy;
using namespace daisysp;

class Synth
{
public:
    static Synth* Create(std::shared_ptr<EvQueue> q, float sampleRate) {
        if (!instance_) {
            instance_ = std::make_unique<Synth>(q, sampleRate);
        }

        return instance_.get();
    }

    static Synth* Instance() {
        return instance_.get();
    }

    Synth(std::shared_ptr<EvQueue> q, float sampleRate) :
        freq{1000},
        amp{0.25},
        evq{q}
    {
        osc.Init(sampleRate);
    }
    ~Synth() = default;

    // This must be static to assign to the hardware handler.
    static void AudioCallback(AudioHandle::InputBuffer  in,
                       AudioHandle::OutputBuffer out,
                       size_t                    size);

    void NoteOn(uint8_t note, uint8_t vel);
    void NoteOff(uint8_t note);

    void setFreq(float f);
    void setAmp(float a);

private:

    void Process(float *sampleL, float *sampleR, size_t size);

    Oscillator osc;

    float freq;
    float amp;

    std::shared_ptr<EvQueue> evq;

    static std::unique_ptr<Synth> instance_;
};
