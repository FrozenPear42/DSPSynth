#include "tt.h"
#include "midi2spi.h"
#include "envelope.h"
#include "effect.h"
#include "oscillator.h"
#include "filter.h"
#include <math.h>

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
#define CC_RES 27

#define LFO_MAX_FREQ 40.0
#define DIST_GAIN_MAX 100.0
#define TIME_MAX 1.0

float MIDICCparams[256];

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

void initSynth()
{
    moogFilterInit(&moogFilterDesc);
    n = 0;
    pitchA = 440;
    envelopeClear(&envelopeA);
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

    SineOscillator(VCOTMPbuffer, N_SAMPLES, n, pitchA);
    for (i = 0; i < N_SAMPLES; i++)
        VCObuffer[i] += VCOTMPbuffer[i] * MIDICCparams[CC_MIX_SINE];

    SquareOscillator(VCOTMPbuffer, N_SAMPLES, n, pitchA);
    for (i = 0; i < N_SAMPLES; i++)
        VCObuffer[i] += VCOTMPbuffer[i] * MIDICCparams[CC_MIX_SQUARE];

    SawtoothOscillator(VCOTMPbuffer, N_SAMPLES, n, pitchA);
    for (i = 0; i < N_SAMPLES; i++)
        VCObuffer[i] += VCOTMPbuffer[i] * MIDICCparams[CC_MIX_SAWTOOTH];

    TriangleOscillator(VCOTMPbuffer, N_SAMPLES, n, pitchA);
    for (i = 0; i < N_SAMPLES; i++)
        VCObuffer[i] += VCOTMPbuffer[i] * MIDICCparams[CC_MIX_TRIANGLE];

    // LFO
    SineOscillator(LFObuffer, N_SAMPLES, n, LFO_MAX_FREQ * MIDICCparams[CC_LFO_FREQ]);

    // VCO modulation with LFO
    for (i = 0; i < N_SAMPLES; i++)
        VCObuffer[i] = VCObuffer[i] * LFObuffer[i];

    // filtration
    moogFilter(&moogFilterDesc, VCObuffer, outputBuffer, N_SAMPLES, MIDICCparams[CC_CUTOFF], MIDICCparams[CC_RES]);
    //for (i = 0; i < N_SAMPLES; i++)
    //	outputBuffer[i] = VCObuffer[i];

    // envelope
    envelopeStep(&envelopeA, envelopeBuffer, N_SAMPLES);
    for (i = 0; i < N_SAMPLES; i++)
    {
        outputBuffer[i] = outputBuffer[i] * envelopeBuffer[i];
    }

    //distortion
    distortion(outputBuffer, N_SAMPLES, DIST_GAIN_MAX * MIDICCparams[CC_DISTORTION]);

    //pan
    for (i = 0; i < N_SAMPLES; i++)
    {
        outputBufferLeft[i] = MIDICCparams[CC_MASTER] * (1 - MIDICCparams[CC_PAN]) * outputBuffer[i];
        outputBufferRight[i] = MIDICCparams[CC_MASTER] * (MIDICCparams[CC_PAN]) * outputBuffer[i];
    }

    // for (i = 0; i < NUM_SAMPLES; i++)
    // {
    //     is = *ptr;
    //     if (is & 0x800000)
    //         is |= 0xFF000000;
    //     *ptr++ = is;
    // }
    // ptr = block_ptr;
    // for (i = 0; i < NUM_SAMPLES / 2; i++)
    // {
    //     *ptr++ = (int)(*ptr * (int)vr) >> 7;
    //     *ptr++ = (int)(*ptr * (int)vl) >> 7;
    // }

    for (i = 0; i < NUM_SAMPLES; i++)
    {
        block_ptr[2 * i]  = (int)(outputBufferLeft[i] * 8388608.0);
        block_ptr[2 * i + 1] = (int)(outputBufferRight[i] * 8388608.0);
    }

    //next buffer
    n = n + N_SAMPLES;

    //Clear the Processing Active Semaphore after processing is complete
    isProcessing = 0;
}
