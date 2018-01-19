#ifndef _ENVELOPE_H_
#define _ENVELOPE_H_

#define ENVELOPE_RISE 0
#define ENVELOPE_DECAY 1

typedef struct
{
	float currentValue;
	float attackCoeff;
	float decayCoeff;
	float highLevel;
	unsigned char state;
} envelope_t;

void envelopeClear(envelope_t *envelope);
void envelopeInit(envelope_t *envelope, long samplingFrequency, float lowLevel, float highLevel, float riseTime, float decayTime);
void envelopeChangeState(envelope_t* envelope, unsigned char state);
void envelopeStep(envelope_t *envelope, float *buffer, long bufferSize);

#endif
