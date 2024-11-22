#ifndef SLTK_WAVENC_H_
#define SLTK_WAVENC_H_

#define WAV_FORMAT_LPCM 0x1
#define WAV_FORMAT_IMA_ADPCM 0x11

typedef struct {
    /*
     * only support LPCM and adpcm
     */
    int fmt;
    /*
     * sampling rate in Hz
     */
    int sample_rate;
    /*
     * channel number
     */
    int chnum;
} wavenc_prm_t;


void *wavenc_open(encoder_init_t * init);
int wavenc_proc(void *handle);
int wavenc_cmd(void *handle, uint32_t cmd, uint32_t arg);
void wavenc_close(void *handle);

#endif
