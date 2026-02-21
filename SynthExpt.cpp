/** Simple example of using USB MIDI
 *
 *  When the project boots up, a 100Hz sine wave will emit from both outputs,
 *  and the Daisy should appear as an Audio/MIDI device on a connected host.
 *
 *  To keep the example short, only note on messages are handled, and there
 *  is only a single oscillator voice that tracks the most recent note message.
 */
#include "daisy_seed.h"
#include "daisysp.h"

#include "synth.h"
#include "MidiManager.h"
#include "EventQueue.h"

#include <memory>

using namespace daisy;
using namespace daisysp;

DaisySeed      hw;
MidiUsbHandler midi;
Oscillator     osc;

std::unique_ptr<Synth> gSynth;
EvQueue gEvQueue;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    for (size_t i=0; i < size; i++) {
        gSynth->Process(&out[1][i]);
        out[0][i] = out[1][i];
    }
}

int main(void)
{
    /** Basic initialization of Daisy hardware */
    hw.Configure();
    hw.Init();

    /** Initialize USB Midi
		 *  by default this is set to use the built in (USB FS) peripheral.
		 *
		 *  by setting midi_cfg.transport_config.periph = MidiUsbTransport::Config::EXTERNAL
		 *  the USB HS pins can be used (as FS) for MIDI
		 */
    MidiUsbHandler::Config midi_cfg;
    midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
    midi.Init(midi_cfg);

    std::shared_ptr<EvQueue> evq = std::make_shared<EvQueue>();

    MidiManager midiMgr(evq);

    /** Initialize our test tone */
    gSynth = std::make_unique<Synth>(evq, hw.AudioSampleRate());

    /** And start the audio callback */
    hw.StartAudio(AudioCallback);
    while(1)
    {
        /** Listen to MIDI for new changes */
        midi.Listen();

        /** When there are messages waiting in the queue... */
        while(midi.HasEvents())
        {
            /** Pull the oldest one from the list... */
            auto msg = midi.PopEvent();
            midiMgr.HandleMidiMessage(msg);

        //     switch(msg.type)
        //     {
        //         case NoteOn:
        //         {
        //             /** and change the frequency of the oscillator */
        //             auto note_msg = msg.AsNoteOn();
        //             if(note_msg.velocity != 0)
        //                 gSynth->setFreq(mtof(note_msg.note));
        //         }
        //         break;
        //             // Since we only care about note-on messages in this example
        //             // we'll ignore all other message types
        //         default: break;
        //     }
        }
    }
}
