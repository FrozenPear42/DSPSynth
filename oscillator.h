#ifndef _OSCILLATOR_H_
#define _OSCILLATOR_H_

#define SAMPLING_FREQ 48000.0
#define M_PI 3.14159265358979323846

void SineOscillator(float* buffer, int bufferSize, long sample, float frequency);
void SquareOscillator(float* buffer, int bufferSize, long sample, float frequency);
void TriangleOscillator(float* buffer, int bufferSize, long sample, float frequency);
void SawtoothOscillator(float* buffer, int bufferSize, long sample, float frequency);

#endif
