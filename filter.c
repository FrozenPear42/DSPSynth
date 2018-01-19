#include "filter.h"

void moogFilterInit(moog_filter_t *filter)
{
    filter->in[0] = 0;
    filter->in[1] = 0;
    filter->in[2] = 0;
    filter->in[3] = 0;
    filter->out[0] = 0;
    filter->out[1] = 0;
    filter->out[2] = 0;
    filter->out[3] = 0;
}

void moogFilter(moog_filter_t *filter, float *input, float *output, long bufferSize, float fc, float res)
{
    int i;
    float tmp;
    float f = fc * 1.16;
    float f2 = f * f;
    float fb = 4 * res * (1.0 - 0.15 * f2);
    for (i = 0; i < bufferSize; i++)
    {
        tmp = input[i] - filter->out[3] * fb;
        tmp *= 0.35013 * f2 * f2;
        filter->out[0] = tmp            + 0.3 * filter->in[0] + (1 - f) * filter->out[0]; // Pole 1
        filter->in[0] = tmp;
        filter->out[1] = filter->out[0] + 0.3 * filter->in[1] + (1 - f) * filter->out[1]; // Pole 2
        filter->in[1] = filter->out[0];
        filter->out[2] = filter->out[1] + 0.3 * filter->in[2] + (1 - f) * filter->out[2]; // Pole 3
        filter->in[2] = filter->out[1];
        filter->out[3] = filter->out[2] + 0.3 * filter->in[3] + (1 - f) * filter->out[3]; // Pole 4
        filter->in[3] = filter->out[2];
        output[i] = filter->out[3];
    }
}
