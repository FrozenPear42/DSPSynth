#ifndef FILTER_H_
#define FILTER_H_

typedef struct
{
    float in[4];
    float out[4];
} moog_filter_t;

void moogFilterInit(moog_filter_t *filter);
void moogFilter(moog_filter_t *filter, float *input, float *output, long bufferSize, float fc, float res);

#endif

