#ifndef MIDI2SPI_H_
#define MIDI2SPI_H_

#define IGNORE_REC 0
#define CC_REC 1
#define NOTEON_REC 2
#define NOTEOFF_REC 5
#define CC_SECOND_REC 3
#define NOTEON_SECOND_REC 4
#define NOTEOFF_SECOND_REC 6

extern float MIDICCparams[256];

#endif

