#include "oscillator.h"
#include "effect.h"
#include <math.h>

void SineOscillator(float *buffer, int bufferSize, long sample, float frequency)
{
    int i;
    float f = frequency / SAMPLING_FREQ;

    for (i = 0; i < bufferSize; i++)
        buffer[i] = sin((2 * M_PI * f * (i + sample)));
}

void SquareOscillator(float *buffer, int bufferSize, long sample, float frequency)
{
    SineOscillator(buffer, bufferSize, sample, frequency);
    distortion(buffer, bufferSize, 10000);
}

void SawtoothOscillator(float *buffer, int bufferSize, long sample, float frequency)
{
    SineOscillator(buffer, bufferSize, sample, frequency);
}

void TriangleOscillator(float *buffer, int bufferSize, long sample, float frequency)
{
    SineOscillator(buffer, bufferSize, sample, frequency);
}

