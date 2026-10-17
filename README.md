# SynthExpt

SynthExpt is a Daisy Seed synthesizer built with libDaisy and DaisySP. It receives USB MIDI, generates audio in the audio callback, and duplicates the resulting signal to the left and right outputs.

## Signal path

The synth combines:

1. A square-wave oscillator and a saw-wave oscillator, mixed equally.
2. A shared ADSR envelope and LFO-based amplitude modulation.
3. A Moog ladder filter.
4. A smoothed master-amplitude control.

Both oscillators follow the current MIDI note. Oscillator 2 is fixed at 1.01 times the note frequency. The LFO is a sine wave. Audio processing and MIDI handling communicate through an event queue; parameter changes are shared through `ParameterState`.

## MIDI notes

Note On and Note Off messages are handled on any MIDI channel:

- **Note On:** Adds the note to the held-note list (up to eight notes). Its velocity is converted to an amplitude. The envelope is triggered when the first note is played.
- **Note Off:** Removes that note. The envelope enters its release phase when no notes remain.
- When multiple notes are held, the most recently stored held note determines pitch. Changing notes while a note is already held does not retrigger the envelope.

## MIDI CC assignments

CC values are normalized from 0–127 and mapped to these parameter ranges:

| CC | Parameter | Range | Mapping |
|---:|---|---:|---|
| 11 | Oscillator 1 amplitude | 0–1 | Linear |
| 12 | Oscillator 2 amplitude | 0–1 | Linear |
| 13 | Attack | 0.005–10 s | Logarithmic |
| 14 | Decay | 0.005–10 s | Logarithmic |
| 15 | Sustain | 0–1 | Linear |
| 16 | Release | 0.005–15 s | Logarithmic |
| 17 | Filter cutoff | 100–10,000 Hz | Logarithmic |
| 18 | Filter resonance | 0–0.8 | Linear |
| 19 | LFO frequency | 0–20 Hz | Linear |
| 20 | LFO depth | 0–1 | Linear |
| 21 | Master amplitude | 0–1 | Linear |

Other CCs are intended to be ignored. **Current implementation caveat:** the CC lookup table initializes only its first entry to `-1`; other unassigned entries default to `0`. Consequently, unassigned CCs currently map to `Osc1Amp` instead of being ignored.

## Implementation notes

- `SynthMain.cpp` initializes USB MIDI, the parameter state, event queue, synth, and audio callback.
- `MidiManager` translates MIDI note and CC messages. CC changes are committed to `ParameterState`.
- `Synth::Process` consumes note events and generates audio. The same filtered sample is written to both output channels.
- The oscillator amplitude CCs are declared and mapped, but `Synth` currently sets both oscillator amplitudes to `1.0` and does not read those parameters. Their CCs therefore have no audible effect.
- The code shown does not define additional MIDI controls such as pitch bend or modulation wheel.
