import math

print("constexpr float kMidiNoteToFreq[128] = {")

for n in range(128):
    freq = 440.0 * (2 ** ((n - 69) / 12.0))
    print(f"    {freq:.6f}f,", end="")

    if (n + 1) % 4 == 0:
        print()

print("};")
