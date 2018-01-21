#ifndef _OSCILLATOR_H_
#define _OSCILLATOR_H_

#define SAMPLING_FREQ 48000.0
#define M_PI 3.14159265358979323846

typedef struct
{
    float frequency;
    float phaseDelta;
} sine_oscillator_t;

typedef struct
{
    sine_oscillator_t sine;
} square_oscillator_t;

typedef struct
{
    sine_oscillator_t sine;
} triangle_oscillator_t;

typedef struct
{
    float frequency;
    long phaseDelta;
} sawtooth_oscillator_t;

void InitSine(sine_oscillator_t *sine);
void InitSquare(square_oscillator_t *square);
void InitTriangle(triangle_oscillator_t *triagle);
void InitSawtooth(sawtooth_oscillator_t *saw);

void SineOscillator(sine_oscillator_t* sine, float *buffer, int bufferSize, long sample, float frequency);
void SquareOscillator(square_oscillator_t* square, float *buffer, int bufferSize, long sample, float frequency);
void TriangleOscillator(triangle_oscillator_t* triangle, float *buffer, int bufferSize, long sample, float frequency);
void SawtoothOscillator(sawtooth_oscillator_t* sawtooth, float *buffer, int bufferSize, long sample, float frequency);

#endif
