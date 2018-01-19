#include "tt.h"

#define IGNORE_REC 0
#define CC_REC 1
#define NOTEON_REC 2
#define CC_SECOND_REC 3
#define NOTEON_SECOND_REC 4


unsigned char params[256];

unsigned char notes[VOICES];
unsigned char notes_velocity[VOICES];
unsigned char notes_last_index;

unsigned char liczba;
unsigned char byte_2nd;
unsigned char byte_3rd;
unsigned char indeks_midi;
unsigned char index;

void handle_LED(int);

void InitSPI()
{
    int i;
    index = 0;
    /* First configure the SPI Control registers */
    /* First clear a few registers     */
    
    for (i=0;i<256;i++)
    	params[i] = 0;
    
    for(i=0;i< VOICES;i++){
        notes[i] = 0;
        notes_velocity[i] = 0;
    }
    notes_last_index = 0;

    *pSPICTL = (TXFLSH | RXFLSH);
//    *pSPIFLG = 0;

  
    /* Setup the SPI Flag register to FLAG3 : 0xF708*/
    //*pSPIFLG = 0xFF00;

    
    /*
    	PIEN - enable SPI
    	MSBF - begin with MSB
    	WL16 - word length set to 16bits
    	TIMOD1 - send when there's data incoming
    	SENDZ - send zeros if there was no data to be sent
    	ISSEN - enable SPI activation with SS
    	CLKPL - change data polarity
    */
    
    
    *pSPICTL = (SPIEN | MSBF | WL16 | CPHASE);//TIMOD1 | SENDZ | ISSEN | CLKPL);
    handle_LED(0);
}

void SpiISR(int sig_int)
{
    /*
    unsigned int rdata = *pRXSPI;
    unsigned int index = (rdata >> 8) & 0xff;
    params[index] = rdata & 0xff;
    *pTXSPI = index;
    */
    
    index++;
    
    unsigned int rdata = *pRXSPI;
    rdata = rdata & 0x00FF;
    //params[index]=rdata;
    
    if (rdata > 127) {
        liczba = rdata & 0xF0;
        if (liczba == 0xB0) {//polecenie controll change, dowolny adres MIDI       
            indeks_midi = CC_REC; //nastepny odebrany bajt <128 wpisz do 2nd_byte 
        }  else if( liczba == 0x90) { //polecenie noteon
            indeks_midi = NOTEON_REC;           
        } else {
            indeks_midi = IGNORE_REC; //polecenie inne niz controll change, ignoruj
        }
    } else {
        if (indeks_midi == CC_REC) {
            byte_2nd = rdata;
            indeks_midi = CC_SECOND_REC; //nastepny odebrany bajt <128 wpisz do 3rd_byte
        
        } else if (indeks_midi == NOTEON_REC) {
            byte_2nd = rdata;
            indeks_midi = NOTEON_SECOND_REC; //nastepny odebrany bajt <128 wpisz do 3rd_byte
        
        } else if (indeks_midi == CC_SECOND_REC) {
            byte_3rd = rdata;
            indeks_midi = CC_REC; //jesli odbierze potem jeszcze jeden bajt <128, to kolejna dana
            handle_LED(byte_3rd-64);
            params[byte_2nd] = byte_3rd;
        
        } else if (indeks_midi == NOTEON_SECOND_REC) {
            byte_3rd = rdata;
            indeks_midi = NOTEON_REC; //jesli odbierze potem jeszcze jeden bajt <128, to kolejna dana
            if(byte_3rd != 0) {
                notes[notes_last_index] = byte_2nd;
                notes_velocity[notes_last_index] = byte_3rd;
                notes_last_index = (notes_last_index+1) % VOICES;
            }
            else {
                for(i=0; i< VOICES;i++)
                    if(notes[i] == byte_2nd)
                        notes_velocity[i] = 0;
            }
        }
    }
}

void DisableSPI()
{
    *pSPICTL = (TXFLSH | RXFLSH);
}

void handle_LED(int led_value){
//lights as described at the top of the file
    *pPPCTL=0;

    *pIIPP=(int) &led_value;
    *pIMPP=1;
    *pICPP=1;
    *pEMPP=1;
    *pECPP=1;
    *pEIPP=0x400000;

    *pPPCTL=PPTRAN|PPBHC|PPDUR20|PPDEN|PPEN;
}
