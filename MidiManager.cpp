#include "MidiManager.h"
#include "SynthParams.h"
#include "EventQueue.h"

#include "daisysp.h"

static int8_t kCcToParam[128] = {-1};


MidiManager::MidiManager(ParameterState& params, EvQueue& q)
: params_{params}, evq{q}
{
    initCcMap();
}

void MidiManager::HandleMidiMessage(daisy::MidiEvent m)
{
    switch(m.type)
    {
        case daisy::NoteOn:
        {
            daisy::NoteOnEvent p = m.AsNoteOn();
            onNoteOn(p.note, p.velocity);
            break;
        }
        case daisy::NoteOff:
        {
            daisy::NoteOnEvent p = m.AsNoteOn();
            onNoteOff(p.note);
            break;
        }
        case daisy::ControlChange:
        {
            daisy::ControlChangeEvent p = m.AsControlChange();
            onHandleCc(p.control_number, p.value);
            break;
        }
        default: return;
    }
}

void MidiManager::onNoteOn(uint8_t note, uint8_t vel)
{
    float amp = daisysp::fmap(vel / 127.f, 0.f, 1.f, daisysp::Mapping::EXP);
    evq.push({EV_NOTE_ON, note, amp});
}

void MidiManager::onNoteOff(uint8_t note)
{
    evq.push({EV_NOTE_OFF, note, 0});
}

constexpr ParamDescriptor kParamDescriptors[] = {
    {0.0f, 1.0f, daisysp::Mapping::LINEAR},    // MasterAmplitude
    {0.005f, 10.0f, daisysp::Mapping::LOG},    // Attack
    {0.005f, 10.0f, daisysp::Mapping::LOG},    // Decay
    {0.0f, 1.0f, daisysp::Mapping::LINEAR},    // Sustain
    {0.005f, 15.0f, daisysp::Mapping::LOG},    // Release
    {100.0f, 10000.0f, daisysp::Mapping::LOG}, // Filter Cutoff
    {0.0f, 0.8f, daisysp::Mapping::LINEAR},    // Filter Resonance
    {0.0f, 20.0f, daisysp::Mapping::LINEAR},   // LFO Frequency
    {0.f, 1.0f, daisysp::Mapping::LINEAR}      // LFO Depth
};

void MidiManager::initCcMap()
{
    /* 0–10 unused = -1 */
    kCcToParam[11] = (int8_t)ParamID::MasterAmplitude;
    kCcToParam[12] = (int8_t)ParamID::Attack;
    kCcToParam[13] = (int8_t)ParamID::Decay;
    kCcToParam[14] = (int8_t)ParamID::Sustain;
    kCcToParam[15] = (int8_t)ParamID::Release;
    kCcToParam[16] = (int8_t)ParamID::FilterCutoff;
    kCcToParam[17] = (int8_t)ParamID::FilterReson;
    kCcToParam[18] = (int8_t)ParamID::LfoFreq;
    kCcToParam[19] = (int8_t)ParamID::LfoDepth;
};

void MidiManager::onHandleCc(uint8_t cc, uint8_t value)
{
    int8_t pid = kCcToParam[cc];
    if(pid < 0)
        return;

    auto& p = params_.GetWriteParams();

    const auto& desc = kParamDescriptors[pid];

    float normalized = value / 127.f;
    p.values[pid] = daisysp::fmap(normalized, desc.min, desc.max, desc.mapping);

    dirty_ = true;
}

void MidiManager::CommitIfDirty()
{
    if(dirty_)
    {
        params_.Commit();
        dirty_ = false;
    }
}
