#ifndef SLTK_CVSD_ENC_H_
#define SLTK_CVSD_ENC_H_

void* cvsd_encoder_open();
void cvsd_encoder_close(void* encoder);
int cvsd_encoder_proc(void* encoder, short* input, char* output, int Len);

#endif
