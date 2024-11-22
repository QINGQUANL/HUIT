#ifndef _SLTK_KWS_VAD_H_
#define _SLTK_KWS_VAD_H_
#define OUTPUT_DIMEN (2)



typedef struct
{
    int use_power;
    int take_log;

}feat_fbank_cfg;


typedef struct
{
    /* weight prms */
    const float* weight;
    /* bias prms */
    const float* bias;
    /* layer index */
    int lidx;

}dnn_layer_config_t;


typedef struct
{
    /* feature vector size */
    short feat_dim;
    /* feature arry time dim of input to dnn */
    short input_dim;
    /* previous of cur time dim of input to dnn */
    short prev_dim;
    /* hidden layer dim */
    short hidden_dim;
    /* output layer dim */
    short output_dim;
    /* hop size in ms */
    short hop_size;
    /* frame_size in ms*/
    short frame_size;
    /* sampling frequency */
    int fs;
    /* const window table for feature extration */
    const float* win;

}kws_initprm_t;



#define INPUT_DIMEN (20*11)
#define LAYER_DIMEN (64)
#define OUTPUT_DIMEN (2)

#define CMD_FEXT_SET_CONFIG     0x40
#define CMD_DNN_SET_LAYERPRM    0x80

extern const float w0[LAYER_DIMEN][INPUT_DIMEN];
extern const float b0[LAYER_DIMEN];
extern const float w1[LAYER_DIMEN][LAYER_DIMEN];
extern const float b1[LAYER_DIMEN];
extern const float w2[LAYER_DIMEN][LAYER_DIMEN];
extern const float b2[LAYER_DIMEN];
extern const float w3[OUTPUT_DIMEN][LAYER_DIMEN];
extern const float b3[OUTPUT_DIMEN];
extern const float symmetric_hamm[100];


void sltk_kws_close(void* handle);

void* sltk_kws_open(kws_initprm_t* prm);

float* sltk_kws_frameproc(void* handle, short* input);

int sltk_kws_cmd(void* handle, unsigned int cmd, unsigned int arg);

#endif