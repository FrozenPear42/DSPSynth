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
#define DIST_GAIN_MAX 500.0
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

unsigned char noteA;

moog_filter_t moogFilterDesc;
envelope_t envelopeA;


float pitchTable[128] = {
    8.175798915643707, 8.661957218027252, 9.177023997418987, 9.722718241315029, 10.300861153527185, 10.913382232281371,
    11.562325709738575, 12.249857374429665, 12.978271799373285, 13.75, 14.56761754744031, 15.433853164253879, 16.351597831287414,
    17.323914436054505, 18.354047994837973, 19.445436482630058, 20.60172230705437, 21.826764464562743, 23.12465141947715,
    24.49971474885933, 25.95654359874657, 27.5, 29.13523509488062, 30.867706328507758, 32.70319566257483, 34.64782887210901,
    36.70809598967595, 38.890872965260115, 41.20344461410874, 43.653528929125486, 46.2493028389543, 48.99942949771866, 51.91308719749314,
    55.0, 58.27047018976124, 61.7354126570155, 65.40639132514966, 69.29565774421802, 73.41619197935188, 77.78174593052023,
    82.4068892282175, 87.30705785825097, 92.4986056779086, 97.99885899543733, 103.82617439498628, 110.0, 116.54094037952248,
    123.470825314031, 130.8127826502993, 138.59131548843604, 146.83238395870376, 155.56349186104046, 164.813778456435,
    174.61411571650194, 184.9972113558172, 195.99771799087466, 207.65234878997256, 220.0, 233.0818807590449, 246.94165062806212,
    261.6255653005986, 277.1826309768721, 293.66476791740763, 311.1269837220809, 329.62755691286986, 349.22823143300394,
    369.9944227116344, 391.9954359817492, 415.3046975799452, 440.0, 466.1637615180898, 493.88330125612424, 523.2511306011972,
    554.3652619537442, 587.3295358348153, 622.2539674441618, 659.2551138257397, 698.4564628660079, 739.9888454232688,
    783.9908719634984, 830.6093951598904, 880.0, 932.3275230361796, 987.7666025122485, 1046.5022612023945, 1108.7305239074883,
    1174.6590716696305, 1244.5079348883237, 1318.5102276514795, 1396.9129257320158, 1479.9776908465376, 1567.9817439269968,
    1661.2187903197807, 1760.0, 1864.6550460723593, 1975.533205024497, 2093.004522404789, 2217.4610478149766, 2349.318143339261,
    2489.0158697766474, 2637.020455302959, 2793.8258514640315, 2959.955381693075, 3135.9634878539937, 3322.4375806395615,
    3520.0, 3729.3100921447212, 3951.0664100489917, 4186.009044809578, 4434.922095629955, 4698.636286678519,
    4978.031739553295, 5274.040910605921, 5587.65170292806, 5919.91076338615, 6271.926975707992, 6644.875161279119, 7040.0,
    7458.6201842894425, 7902.132820097983, 8372.018089619156, 8869.84419125991, 9397.272573357039, 9956.06347910659, 10548.081821211843,
    11175.30340585612, 11839.8215267723};


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

	envelopeClear(&envelopeA);
	noteA = 0;
	}


void activateNote(unsigned char note, float velocity)
{
	//if(noteA == note)
	//	return;
	noteA = note;
	pitchA = pitchTable[note];
    envelopeInit(&envelopeA, SAMPLING_FREQ, 0.01, velocity, MIDICCparams[CC_ATTACK] * TIME_MAX, MIDICCparams[CC_DECAY] * TIME_MAX);
}

void turnDownNote(unsigned char note)
{
	if(noteA == note){
    	envelopeChangeState(&envelopeA, ENVELOPE_DECAY);
		noteA = 0;
    }
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
			SineOscillator(LFObuffer, N_SAMPLES, n, LFO_MAX_FREQ * MIDICCparams[CC_LFO_FREQ]);
		
		
		// mixing VCO
		SineOscillator(VCObuffer, N_SAMPLES, n, pitchA);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] = VCObuffer[i] * LFObuffer[i] * MIDICCparams[CC_MIX_SINE];
		}

		SquareOscillator(VCOTMPbuffer, N_SAMPLES, n, pitchA);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCOTMPbuffer[i] * LFObuffer[i] * MIDICCparams[CC_MIX_SQUARE];
		}

		SawtoothOscillator(VCOTMPbuffer, N_SAMPLES, n, pitchA);
		for (i = 0; i < N_SAMPLES; i++)
		{
			VCObuffer[i] += VCOTMPbuffer[i] * LFObuffer[i] * MIDICCparams[CC_MIX_SAWTOOTH];
		}

		TriangleOscillator(VCOTMPbuffer, N_SAMPLES, n, pitchA);
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
