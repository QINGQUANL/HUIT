#ifndef SLTK_CVSD_DEC_H_
#define SLTK_CVSD_DEC_H_

void* cvsd_decoder_open();
void cvsd_decoder_close(void* decoder);
int cvsd_decoder_proc(void* decoder, char* input, short* output, int Len);

#endif
