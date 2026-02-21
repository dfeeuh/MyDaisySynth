#include "synth.h"

void Synth::Process(float* sample) {
    *sample = osc.Process();
}

void Synth::setFreq(float f) {
    freq = f;
    osc.SetFreq(freq);
}

void Synth::setAmp(float a) {
    amp = a;
    osc.SetAmp(amp);
}
