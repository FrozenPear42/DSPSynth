#include <filter.h>
#include <math.h>
#include "envelope.h"
#include "effect.h"
#include "oscillator.h"

#define N_SAMPLES 256
#define SAMPLING_FREQ 48000.0

#define CC_MIX_SINE 16
#define CC_MIX_SQUARE 17
#define CC_MIX_SAWTOOTH 18
#define CC_MIX_TRIANGLE 19
#define CC_LFO_FREQ 20
#define CC_DISTORTION 21
#define CC_MASTER 22
#define CC_PAN 23

#define CC_ATTACK 24
#define CC_DECAY 25
#define CC_CUTOFF 26

#define LFO_MAX_FREQ 40.0
#define DIST_GAIN_MAX 8.0
#define TIME_MAX 1.0

float MIDICCparams[256];

float LFObuffer[N_SAMPLES];

float VCObuffer[N_SAMPLES];
float VCOTMPbuffer[N_SAMPLES];
float envelopeBuffer[N_SAMPLES];

float outputBuffer[N_SAMPLES];
float outputBufferLeft[N_SAMPLES];
float outputBufferRight[N_SAMPLES];

envelope_t envelopeA;

main()
{
	int i;
	unsigned long n = 0;
	MIDICCparams[CC_MIX_SINE] = 0.6;
	MIDICCparams[CC_MIX_SQUARE] = 0.2;
	MIDICCparams[CC_MIX_SAWTOOTH] = 0.8;
	MIDICCparams[CC_MIX_TRIANGLE] = 0.2;

	MIDICCparams[CC_LFO_FREQ] = 0.5;
	MIDICCparams[CC_DISTORTION] = 0.3;
	MIDICCparams[CC_PAN] = 0.5;
	MIDICCparams[CC_MASTER] = 0.5;

	
	MIDICCparams[CC_ATTACK] = 0.4;
	MIDICCparams[CC_DECAY] = 0.1;
	

	envelopeInit(&envelopeA, SAMPLING_FREQ, 0.01, 1.00, MIDICCparams[CC_ATTACK] * TIME_MAX, MIDICCparams[CC_DECAY] * TIME_MAX);

	while (1)
	{
		// mixing VCO
		SineOscillator(VCOTMPbuffer, N_SAMPLES, n, 1000);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCOTMPbuffer[i] * MIDICCparams[CC_MIX_SINE];
		}

		SquareOscillator(VCOTMPbuffer, N_SAMPLES, n, 1000);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCOTMPbuffer[i] * MIDICCparams[CC_MIX_SQUARE];
		}

		SawtoothOscillator(VCOTMPbuffer, N_SAMPLES, n, 1000);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCOTMPbuffer[i] * MIDICCparams[CC_MIX_SAWTOOTH];
		}

		TriangleOscillator(VCOTMPbuffer, N_SAMPLES, n, 1000);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCOTMPbuffer[i] * MIDICCparams[CC_MIX_TRIANGLE];
		}

		// LFO
		SineOscillator(LFObuffer, N_SAMPLES, n, LFO_MAX_FREQ * MIDICCparams[CC_LFO_FREQ]);

		// VCO modulation with LFO
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCObuffer[i] * LFObuffer[i];
		}

		// filtration
		// TODO
		// fir(input, output4,coeffs,dline,N,TAPS);

		// envelope
		envelopeStep(&envelopeA, envelopeBuffer, N_SAMPLES);
		for (i = 0; i < N_SAMPLES; i++)
		{
			outputBuffer[i] = VCObuffer[i] * envelopeBuffer[i];
		}

		//distortion
		distortion(outputBuffer, N_SAMPLES, DIST_GAIN_MAX * MIDICCparams[CC_DISTORTION]);

		//master volume

		//pan
		for (i = 0; i < N_SAMPLES; i++)
		{
			outputBufferLeft[i] = MIDICCparams[CC_MASTER] * (1 - MIDICCparams[CC_PAN]) * outputBuffer[i];
			outputBufferRight[i] = MIDICCparams[CC_MASTER] * (MIDICCparams[CC_PAN]) * outputBuffer[i];
		}
		//next buffer
		n = n + N_SAMPLES;
	}
}
