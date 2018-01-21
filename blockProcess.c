#include "tt.h"
#include "midi2spi.h"
#include "envelope.h"
#include "effect.h"
#include "oscillator.h"
#include "filter.h"
#include <math.h>

#define N_SAMPLES 256
#define SAMPLING_FREQ 48000.0

#define CC_MIX_SINE 21
#define CC_MIX_SQUARE 22
#define CC_MIX_SAWTOOTH 23
#define CC_MIX_TRIANGLE 24
#define CC_LFO_FREQ 25
#define CC_DISTORTION 26
#define CC_MASTER 31
#define CC_PAN 32

#define CC_ATTACK 29
#define CC_DECAY 30
#define CC_CUTOFF 27
#define CC_RES 28

#define LFO_MAX_FREQ 20.0
#define DIST_GAIN_MAX 100.0
#define TIME_MAX 1.0

float LFObuffer[N_SAMPLES];

float VCObuffer[N_SAMPLES];
float VCOTMPbuffer[N_SAMPLES];
float envelopeBuffer[N_SAMPLES];

float outputBuffer[N_SAMPLES];
float outputBufferLeft[N_SAMPLES];
float outputBufferRight[N_SAMPLES];
long n;

float pitchA;

moog_filter_t moogFilterDesc;
envelope_t envelopeA;

sine_oscillator_t sine;
square_oscillator_t square;
triangle_oscillator_t triangle;
sawtooth_oscillator_t sawtooth;

void initSynth(void)
{
    moogFilterInit(&moogFilterDesc);
    n = 0;
    pitchA = 440.0;
    envelopeClear(&envelopeA);

	MIDICCparams[CC_MIX_SINE] = 1.0;
	MIDICCparams[CC_MIX_SQUARE] = 0.0;
	MIDICCparams[CC_MIX_SAWTOOTH] = 0.0;
	MIDICCparams[CC_MIX_TRIANGLE] = 0.0;

	MIDICCparams[CC_LFO_FREQ] = 0.0;
	MIDICCparams[CC_DISTORTION] = 0.4;
	MIDICCparams[CC_PAN] = 0.5;
	MIDICCparams[CC_MASTER] = 1.0;
	
	MIDICCparams[CC_ATTACK] = 0.4;
	MIDICCparams[CC_DECAY] = 0.1;

	MIDICCparams[CC_CUTOFF] = 0.9;
	MIDICCparams[CC_RES] = 0.2;

	InitSine(&sine);
	InitSquare(&square);
	InitTriangle(&triangle);
	InitSawtooth(&sawtooth);
    }


void activateNote(float pitch, float velocity)
{
    pitchA = pitch;
    envelopeInit(&envelopeA, SAMPLING_FREQ, 0.01, velocity, MIDICCparams[CC_ATTACK] * TIME_MAX, MIDICCparams[CC_DECAY] * TIME_MAX);
}

void turnDownNote(float pitch)
{
    envelopeChangeState(&envelopeA, ENVELOPE_DECAY);
}

void processBlock(unsigned int *block_ptr)
{
    int i;

    //Clear the Block Ready Semaphore
    blockReady = 0;

    //Set the Processing Active Semaphore before starting processing
    isProcessing = 1;

    *pPPCTL = 0;
	
    int led = int_cntr;
    
    *pIIPP = (int)&led;
    *pIMPP = 1;
    *pICPP = 1;
    *pEMPP = 1;
    *pECPP = 1;
    *pEIPP = 0x400000;

    *pPPCTL = PPTRAN | PPBHC | PPDUR20 | PPDEN | PPEN;
    
		// LFO
		if(MIDICCparams[CC_LFO_FREQ] < 0.00001)
			for(i = 0; i < N_SAMPLES; i++)
				LFObuffer[i] = 1.0;
		else
			SineOscillator(&sine, LFObuffer, N_SAMPLES, n, LFO_MAX_FREQ * MIDICCparams[CC_LFO_FREQ]);
		
		
		// mixing VCO
		SineOscillator(&sine, VCObuffer, N_SAMPLES, n, pitchA);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] = VCObuffer[i] * LFObuffer[i] * MIDICCparams[CC_MIX_SINE];
		}

		SquareOscillator(&square, VCOTMPbuffer, N_SAMPLES, n, pitchA);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCOTMPbuffer[i] * LFObuffer[i] * MIDICCparams[CC_MIX_SQUARE];
		}

		SawtoothOscillator(&sawtooth, VCOTMPbuffer, N_SAMPLES, n, pitchA);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCOTMPbuffer[i] * LFObuffer[i] * MIDICCparams[CC_MIX_SAWTOOTH];
		}

		TriangleOscillator(&triangle, VCOTMPbuffer, N_SAMPLES, n, pitchA);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCOTMPbuffer[i] * LFObuffer[i] * MIDICCparams[CC_MIX_TRIANGLE];
		}
		
		//normalize
		for (i = 0; i < N_SAMPLES; i++)
		{
			float level = 0.1 + MIDICCparams[CC_MIX_SINE] + MIDICCparams[CC_MIX_SQUARE] + MIDICCparams[CC_MIX_SAWTOOTH] + MIDICCparams[CC_MIX_TRIANGLE];
			VCObuffer[i] = VCObuffer[i] / level;
		}
		

		// filtration
		moogFilter(&moogFilterDesc, VCObuffer, outputBuffer, N_SAMPLES, MIDICCparams[CC_CUTOFF], MIDICCparams[CC_RES]);
		//for(i=0;i<N_SAMPLES;i++)
		//	outputBuffer[i] = VCObuffer[i];
		
		// envelope
		envelopeStep(&envelopeA, envelopeBuffer, N_SAMPLES);
		for (i = 0; i < N_SAMPLES; i++)
		{
			outputBuffer[i] = outputBuffer[i] * envelopeBuffer[i];
		}

		//distortion
		distortion(outputBuffer, N_SAMPLES, DIST_GAIN_MAX * MIDICCparams[CC_DISTORTION]);
    //pan and conversion (final AMP)
    for (i = 0; i < N_SAMPLES; i++)
    {
        outputBufferLeft[i] = MIDICCparams[CC_MASTER] * (1 - MIDICCparams[CC_PAN]) * outputBuffer[i];
        outputBufferRight[i] = MIDICCparams[CC_MASTER] * (MIDICCparams[CC_PAN]) * outputBuffer[i];
    	int l = (int)((outputBufferLeft[i]) * 8388608.0);
    	int r = (int)((outputBufferRight[i]) * 8388608.0);
        block_ptr[2*i] = l;
    	block_ptr[2*i+1] = r;
    }

    //next buffer
    n = n + N_SAMPLES;

    
    *pPPCTL = 0;
	
    led = 0x00;
    
    *pIIPP = (int)&led;
    *pIMPP = 1;
    *pICPP = 1;
    *pEMPP = 1;
    *pECPP = 1;
    *pEIPP = 0x400000;

    *pPPCTL = PPTRAN | PPBHC | PPDUR20 | PPDEN | PPEN;
    
    
    //Clear the Processing Active Semaphore after processing is complete
    isProcessing = 0;
}
