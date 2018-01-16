///////////////////////////////////////////////////////////////////////////////////////
//NAME:     tt.h
//DATE:     7/15/04
//PURPOSE:  Header file with definitions use in the C-based talkthrough examples
//
////////////////////////////////////////////////////////////////////////////////////////
#ifndef _TT_H_
#define _TT_H_

#include <cdef21364.h>
#include <def21364.h>
#include <signal.h>

#define NUM_SAMPLES 1024
#define SAMPLING_FREQUENCY 48000
#define VOICES 1

// Function prototypes for this talkthrough code

extern void InitPLL(void);
extern void processBlock(unsigned int *);

extern void InitDAI(void);
extern void Init1835viaSPI(void);

extern void InitSPORT(void);
extern void TalkThroughISR(int);
extern void ClearSPORT(void);

extern void SetupSPI1835 () ;
extern void DisableSPI1835 () ;
extern void Configure1835Register (int i) ;
extern unsigned int Get1835Register (int i) ;

extern void SetupIRQ12 () ;
extern void Irq1ISR (int i) ;
extern void Irq2ISR (int i) ;

extern void Delay (int i) ;

extern void SpiISR(int sig_int);
extern void InitSPI();
extern void DisableSPI();

extern unsigned char params[256];
extern unsigned char notes[VOICES];
extern unsigned char notes_velocity[VOICES];
extern unsigned char notes_last_index;

extern int isProcessing;
extern int blockReady;
extern unsigned int *src_pointer[3];
extern int int_cntr;

#endif
