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
    distortion(buffer, bufferSize, 100000);
}

void SawtoothOscillator(float *buffer, int bufferSize, long sample, float frequency)
{
    int i;
    long N = SAMPLING_FREQ / frequency;
    for (i = 0; i < bufferSize; i++)
        buffer[i] = 0.5 - ((i + sample)%N)/(float)N;
}

void TriangleOscillator(float *buffer, int bufferSize, long sample, float frequency)
{
	int i;
    SineOscillator(buffer, bufferSize, sample, frequency);
    for (i = 0; i < bufferSize; i++)
        buffer[i] = 2*asin(buffer[i])/M_PI;
}

