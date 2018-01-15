///////////////////////////////////////////////////////////////////////////////////////
//NAME:     main.c (Block-based Talkthrough)
//DATE:     7/15/04
//PURPOSE:  Talkthrough framework for sending and receiving samples to the AD1835.
//
//USAGE:    This file contains the main routine calls functions to set up the talkthrough
//          routine.
//
////////////////////////////////////////////////////////////////////////////////////////
#include "tt.h"

void SetupIRQ12(void);

void main(void)
{
    
	InitPLL ();
	SetupIRQ12 () ;
    // Need to initialize DAI because the sport signals
    // need to be routed
    InitDAI();

    // This function will configure the codec on the kit
    Init1835viaSPI();

    // Finally setup the sport to receive / transmit the data
    InitSPORT();

    InitSPI();
    
    interrupt (SIG_SP0,TalkThroughISR);
    interrupt (SIG_IRQ1, Irq1ISR) ;
    interrupt (SIG_IRQ2, Irq2ISR) ;
    interrupt (SIG_P1, SpiISR);
	
    // Be in infinite loop and do nothing until done.
    for(;;)
    {
     while(blockReady)
          processBlock(src_pointer[int_cntr]);
    }
    
}
