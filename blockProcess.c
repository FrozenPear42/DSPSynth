#include "tt.h"

// Place the audio processing algorith here. The input and output are given
// as unsigned integer pointers.
extern unsigned char params[256];

void processBlock(unsigned int *block_ptr)
{
    int i;
  	unsigned int* ptr=block_ptr;
    unsigned int is;
   	unsigned char vl=params[16]*(params[18]/127.0);
   	unsigned char vr=params[17]*((127-params[18])/127.0);
   

    
    //Clear the Block Ready Semaphore
    blockReady = 0;
    
    //Set the Processing Active Semaphore before starting processing
    isProcessing = 1;

    
    for(i=0;i<NUM_SAMPLES;i++)
    {
        is = *ptr;
        if (is & 0x800000)
        	is |= 0xFF000000;	    
        *ptr++ = is;
	}
	ptr = block_ptr;
	for(i=0;i<NUM_SAMPLES/2;i++)
	{
	    *ptr++ = (int) (*ptr * (int)vr) >> 7;	    
	    *ptr++ = (int) (*ptr * (int)vl) >> 7;	    
	}
	
    
    //Clear the Processing Active Semaphore after processing is complete
    isProcessing = 0;
}
