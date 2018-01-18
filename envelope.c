#include "envelope.h"
#include <math.h>

void envelopeInit(envelope_t *envelope, long samplingFrequency, float lowLevel, float highLevel, float riseTime, float decayTime)
{
    envelope->currentValue = lowLevel;
    envelope->attackCoeff = (log(highLevel) - log(lowLevel)) / (riseTime * samplingFrequency);
    envelope->decayCoeff = (log(lowLevel) - log(highLevel)) / (decayTime * samplingFrequency);
    envelope->state = ENVELOPE_RISE;
    envelope->highLevel = highLevel;
}

void envelopeChangeState(envelope_t *envelope, unsigned char state)
{
    envelope->state = state;
}

void envelopeStep(envelope_t *envelope, float *buffer, long bufferSize)
{
    int i;
	float coeff = envelope->state == ENVELOPE_RISE ? envelope->attackCoeff : envelope->decayCoeff;
    for (i = 0; i < bufferSize; i++)
    {
        envelope->currentValue += coeff * envelope->currentValue;
        if(envelope->currentValue > envelope->highLevel)
            envelope->currentValue = envelope->highLevel;
        buffer[i] = envelope->currentValue;
    }
}

