#include "effect.h"
#include <math.h>

void distortion(float *buffer, int bufferSize, float gain)
{
    int i;
    for (i = 0; i < bufferSize; i++)
    {
        buffer[i] = buffer[i] * gain;
        float absolute = fabs(buffer[i]);
        buffer[i] = (buffer[i] / absolute) * (1 - exp(-absolute));
    }
}

void gain(float* buffer, int bufferSize,float gain) {
	int i;
    for (i = 0; i < bufferSize; i++)
        buffer[i] = buffer[i] * gain;
}
