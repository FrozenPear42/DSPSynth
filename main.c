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
    // Init clock
	InitPLL ();
	// SetupIRQ12 () ;
	// Wait til clock stabilises
	Delay(20000000);   
    
    // Need to initialize DAI because the sport signals
    // need to be routed
    InitDAI(); 
    
    // Init Codec
    // Wait til DAI stabilises
    Delay(20000000);   
    Init1835viaSPI();
    
    // Finally setup the sport to receive / transmit the data
    InitSPORT();
    
    // This function will configure the codec on the kit
    // Wait til codec initialises
    Delay(20000000);   
    InitSPI();
    
    initSynth();
    handle_LED(0);
    
    interrupt (SIG_SP0,TalkThroughISR);
    interrupt (SIG_P1, SpiISR);

    //    interrupt (SIG_IRQ1, Irq1ISR) ;
	//    interrupt (SIG_IRQ2, Irq2ISR) ;
	
    for(;;)
    {
    	while(!blockReady);
    	processBlock(src_pointer[int_cntr]);
    }
    
}
