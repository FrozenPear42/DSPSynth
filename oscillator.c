#include "oscillator.h"
#include "effect.h"
#include <math.h>

void InitSine(sine_oscillator_t *sine)
{
    sine->frequency = 0;
    sine->phaseDelta = 0;
}

void InitSquare(square_oscillator_t *square)
{
    InitSine(&(square->sine));
}

void InitTriangle(triangle_oscillator_t *triagle)
{
    InitSine(&(triagle->sine));
}
void InitSawtooth(sawtooth_oscillator_t *saw)
{
    saw->frequency = 0;
    saw->phaseDelta = 0;
}

void SineOscillator(sine_oscillator_t *sine, float *buffer, int bufferSize, long sample, float frequency)
{
    int i;

    float f = frequency / SAMPLING_FREQ;
    if (frequency != sine->frequency)
    {
        float fOld = sine->frequency / SAMPLING_FREQ;
        float last = fmod(2 * M_PI * fOld * sample + sine->phaseDelta, 2 * M_PI);
        float current = fmod(2 * M_PI * f * sample, 2 * M_PI);
        sine->phaseDelta = last - current;
        sine->frequency = frequency;
    }
    for (i = 0; i < bufferSize; i++)
        buffer[i] = sin((2 * M_PI * f * (i + sample) + sine->phaseDelta));
}

void SquareOscillator(square_oscillator_t *square, float *buffer, int bufferSize, long sample, float frequency)
{
    SineOscillator(&(square->sine), buffer, bufferSize, sample, frequency);
    distortion(buffer, bufferSize, 100000);
}

void TriangleOscillator(triangle_oscillator_t *triangle, float *buffer, int bufferSize, long sample, float frequency)
{
    int i;
    SineOscillator(&(triangle->sine), buffer, bufferSize, sample, frequency);
    for (i = 0; i < bufferSize; i++)
        buffer[i] = 2 * asin(buffer[i]) / M_PI;
}

void SawtoothOscillator(sawtooth_oscillator_t *sawtooth, float *buffer, int bufferSize, long sample, float frequency)
{
    int i;
    long N = SAMPLING_FREQ / frequency;

    if (frequency != sawtooth->frequency)
    {
        long NOld = SAMPLING_FREQ / sawtooth->frequency;
        float last = (sample + sawtooth->phaseDelta) % NOld;
        float current = sample % N;
        sawtooth->phaseDelta = last - current;
        sawtooth->frequency = frequency;
    }

    for (i = 0; i < bufferSize; i++)
        buffer[i] = 0.5 - ((i + sample + sawtooth->phaseDelta) % N) / (float)N;
}
