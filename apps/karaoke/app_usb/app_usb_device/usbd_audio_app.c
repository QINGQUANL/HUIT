#define LOG_LEV 4
#include "sys_inc.h"
#include "hal.h"
#include "user_config.h"
#include "usbd_audio_app.h"
#include "soundcard.h"
#include "oal.h"
#include "event.h"
#include "usb_bulk_msc.h"
#include "usb_app_config.h"
#include "sltk_rendering_7_1ch_mix.h"
#include "app_psm.h"
#include "ring_buffer.h"

void i2susb_read(void *buf, uint32_t bytes);

AT_SRAM(.usbd_i2s)
void audio_data_combine_to_usb4channel(uint16_t* usb_rec, uint16_t* i2s_rec, uint16_t* mic_buff, int data_length)
{
    for(uint16_t i=0; i< data_length; i++) {
        mic_buff[2*i] = usb_rec[i];
        mic_buff[2*i + 1] = i2s_rec[i];
    }
}

#if (USBD_MODE == USB_MODE_SPEAKER_MIC) || (USBD_MODE == USB_MODE_MIC) || (USBD_MODE == USB_MODE_TWO_AUDIO_CONFIG)

void stereo16_to_mono16(int16_t *pout, const int16_t *pin, int32_t samples);
extern void register_callback(USBD_COMP_StorageTypeDef* storageCb);
extern int8_t usbExtendCommand(uint8_t *params);
int8_t usbAnecbweExtendCommand(uint8_t *params);

#define MIC_BUFF_SIZE   AUDIO_MIC_MPS
#define SPK_BUFF_SIZE   192     //分配一个最大的数据长度

static BOOL trigle = FALSE;

/*
    这个在PC显示的实际音量计算法方式为 xx(db) = MAX_LOGIG_VOLUME(int)* 1/256db
*/
#define MAX_LOGIG_VOLUME    0x64  //最大的逻辑音量，
#define MIN_LOGIG_VOLUME    0x01  //最小的逻辑音量,通常和实际音量有对应关系
#define USB_VOLUME_STEP     2

extern uint8_t usb_ep0buf[64];
extern USBD_SetupReqTypedef setupReq;

uint8_t audio_tmp_buff[192];    //多通道的临时缓存
uint8_t i2s_tmp_buff[192];      //2拖4，从芯片发过来的I2C音频数据

uint8_t* ptr_pcm_data_buff;
uint8_t* ptr_mic_data_buff;

usbdev_context_t *usbdev_ct;
#if USB_AUDIO_DOLBY_SUPPORT_EN
OAL_THREAD r71_task_handle = NULL;
void *r71_cpux_handle = NULL;
static void  *r71 = NULL;
static float *ch_in_cpu0[8];
static float *ch_in_cpu1[8];
#endif
float   usba_proc_inbuffer[2*8*48] = {0};
int16_t usba_proc_outbuffer[2*48] = {0};
uint8_t first_frame = 1;
OAL_SEM usb_r71 = NULL;

uint32_t g_pcmSampleRate = 48000;
uint32_t g_micSampleRate = 48000;
uint32_t g_micUpstreamDataSize = 48* 2 * MIC_INPUT_CHANNALS;

USBD_Audio_CallbackTypedef g_UsbdAudioCb = {
    usbd_audio_system_param_init,
    usbd_init_audio,
    on_prepare_up_stream,
    on_usbd_down_stream,
    on_get_cur,
    on_set_cur,
    on_get_min,
    on_get_max,
    on_get_res,
    on_set_res,
    on_alternate_setting_change,
    ExternAudioCmd,
    ExternAudioData,
};

int8_t compAudioString[36] = {
    0x00,
    0x80,
    0x02,
    0x02,
    (0x25 - 5),   //bug here 0x24
    0x00,
    0x00,
    0x10,
    'S', 'M', 'T', 'L', 'I', 'N', 'K', ' ', /* Manufacturer: 8 bytes  */
    'C', 'O', 'M', 'P', '_', 'A', 'U', 'D', /* Product     : 16 Bytes */
    'I', 'O', ' ', ' ', ' ', ' ', ' ', ' ',
    '3', '.', '0', '0',                      /* Version     : 4 Bytes  */
};

int8_t compAencAudioString[36] = {
    0x00,
    0x80,
    0x02,
    0x02,
    (0x25 - 5),   //bug here 0x24
    0x00,
    0x00,
    0x10,
    'S', 'M', 'T', 'L', 'I', 'N', 'K', ' ', /* Manufacturer: 8 bytes  */
    'A', 'E', 'N', 'C', '_', 'A', 'U', 'D', /* Product     : 16 Bytes */
    'I', 'O', ' ', ' ', ' ', ' ', ' ', ' ',
    '1', '.', '0', '0',                      /* Version     : 4 Bytes  */
};


BOOL is_ready()
{
    return FALSE;
}

BOOL is_write_pr()
{
    return FALSE;
}

USBD_COMP_StorageTypeDef g_compAudioCallback = {
    NULL,
    NULL,
    is_ready,
    is_write_pr,
    0,//msc_read,   //读写函数被重写了
    0,//msc_write,
    NULL,
    (int8_t*)compAudioString,
    NULL
};

void usbd_audio_system_param_init(uint16_t* pcmPacketSize, uint16_t* micPacketSize)
{
    *pcmPacketSize = SPK_BUFF_SIZE;
#if USB_MIC_SR_192000_EN
    *micPacketSize = 192*2* MIC_INPUT_CHANNALS;
#elif USB_MIC_SR_96000_EN
    *micPacketSize = 96*2* MIC_INPUT_CHANNALS;
#else
    *micPacketSize = 48*2* MIC_INPUT_CHANNALS;
#endif
}

void usbd_init_audio(uint32_t _OUT *ptrPcmBuf, uint16_t* _OUT pcmBufLength)
{
    *ptrPcmBuf = (uint32_t)ptr_pcm_data_buff;
    *pcmBufLength = SPK_BUFF_SIZE;
}

int16_t usb_recbuf[48*2*2];

AT_SRAM(.usbd_audio_stereo)
void usbd_audio_sample_bits_change_stereo(uint8_t*  in , uint8_t* out, uint8_t bits)
{
    uint8_t nBytes = bits/8;
    
    for(int i = 0; i < 48*2; i++ ) {    //bit数据量 * 硬件采样率 * 双声道立体声（2）
        out[i* nBytes] = 0x00;
        out[i* nBytes + 1] = in[i*2];
        out[i* nBytes + 2] = in[i*2 + 1];
    }
}

int8_t usb_recbuf_tmp[48* 2 * 2];

AT_SRAM(.usbd_audio_mono)
void usbd_audio_sample_bits_change_mono(uint8_t*  in , uint8_t* out, uint8_t bits)
{
    uint8_t nBytes = bits/8;    
    for(int i = 0; i < 48; i++ ) {
        out[ i* nBytes] = 0x00;
        out[ i* nBytes +1] = in[i*2];
        out[ i* nBytes + 2] = in[i*2 + 1];
    }
}

//TODO:
AT_SRAM(.usbd_audio_r71)
void usbplay_put_deinterleave_8ch_data(int16_t* src, float **ch_in, int32_t samples)
{
    int i;
    for(i = 0; i < samples; i++){
        ch_in[0][i] = (float)src[8*i    ] / 32768.0f;
        ch_in[1][i] = (float)src[8*i + 1] / 32768.0f;
        ch_in[2][i] = (float)src[8*i + 2] / 32768.0f;
        ch_in[3][i] = (float)src[8*i + 3] / 32768.0f;
        ch_in[4][i] = (float)src[8*i + 4] / 32768.0f;
        ch_in[5][i] = (float)src[8*i + 5] / 32768.0f;
        ch_in[6][i] = (float)src[8*i + 6] / 32768.0f;
        ch_in[7][i] = (float)src[8*i + 7] / 32768.0f;
    }
}

AT_SRAM(.usbd_audio_r71)
void usbplay_get_interleave_2ch_data(int16_t* dst, float **ch_in, int32_t samples)
{
    int i, tmpl, tmpr;
    for(i = 0; i < samples; i++){
        tmpl = (int)(ch_in[0][i] * 32767.0f);
        tmpr = (int)(ch_in[1][i] * 32767.0f);

        dst[2*i    ] = tmpl > 32767 ? 32767 : (tmpl < -32768 ? -32768 : tmpl);
        dst[2*i + 1] = tmpr > 32767 ? 32767 : (tmpr < -32768 ? -32768 : tmpr);
    }
}

void usbrec_resample_data_read(void *buf, uint32_t bytes);

AT_SRAM(.usbd_audio)
BOOL on_prepare_up_stream(uint32_t _INOUT *ptrMicBuf, uint16_t* _INOUT length)
{
    *length = g_micUpstreamDataSize;
    
    //static BOOL trigle = FALSE;
    if(usbdev_ct->g_isMicMute) {
        memset(ptr_mic_data_buff, 0, MIC_BUFF_SIZE);
        *ptrMicBuf = (uint32_t)ptr_mic_data_buff;
    } else {

#if 1 || (USB_MIC_CHANNEL == USB_MIC_CHANNEL_MONO) || (USB_MIC_CHANNEL == USB_MIC_CHANNEL_STERO)
        usbrec_resample_data_read(ptr_mic_data_buff, g_micUpstreamDataSize);
        *ptrMicBuf = (uint32_t)ptr_mic_data_buff;
#if 0
#if (USBD_MIC_SAMPLEBIT == USB_MIC_SAMPLE_BITS16)               
            stereo16_to_mono16((int16_t *)ptr_mic_data_buff, 
                (int16_t *)usb_recbuf, 48);
#elif (USBD_MIC_SAMPLEBIT == USB_MIC_SAMPLE_BITS24)   
            stereo16_to_mono16((int16_t *)usb_recbuf_tmp, 
                (int16_t *)usb_recbuf, 48);
             usbd_audio_sample_bits_change_mono((uint8_t*)usb_recbuf_tmp, ptr_mic_data_buff, 24);
#elif (USBD_MIC_SAMPLEBIT == USB_MIC_SAMPLE_BITS32)   
             stereo16_to_mono16((int16_t *)usb_recbuf_tmp, 
                (int16_t *)usb_recbuf, 48);
             usbd_audio_sample_bits_change_mono((uint8_t*)usb_recbuf_tmp, ptr_mic_data_buff, 32);
#endif
#endif
        
#else   //other channel config,4 , 6 , 8
    //logi("reclength %d", MIC_BUFF_SIZE);
    usbrec_read(audio_tmp_buff, 48*2*2);
    i2susb_read(i2s_tmp_buff, 48 *2 *2);
    audio_data_combine_to_usb4channel((uint16_t*)&audio_tmp_buff[0],(uint16_t*)&i2s_tmp_buff[0], (uint16_t*)&ptr_mic_data_buff[0], 96);
    *ptrMicBuf = (uint32_t)ptr_mic_data_buff;
#endif
        trigle = !trigle;
    }
    
    //TODO,  add your code here ---------------
    
    return FALSE;   //FALSE让底层进行传递，TRUE是自己处理
}

AT_SRAM(.usb_addition_hid)
void usbd_on_addition_hid_data_receieve(const uint8_t* hid_data, uint8_t length)
{
    logi("usbd_on_addition_hid_data_receieve");
    printf_array(hid_data, length);
}

#if USB_AUDIO_DOLBY_SUPPORT_EN
void r71_cpu1_process(void *r71_handle)
{
    render_7_1_proc_stage_cpu1(r71_handle, ch_in_cpu1, MIC_BASE_DATA_NUM);
}

void r71_cpu1_init()
{
    //用CPU1调起r71_cpu1_process
    if(r71_cpux_handle == NULL) {
        r71_cpux_handle = OAL_cpux_work_create(1, 0, 256);
    }
    if (r71_cpux_handle) {
        logd("r71 cpu1 init sucessful");
    }else {
        logd("r71 cpu1 init fail");
    }
}

static void *r71_loop(void *handle) 
{
    int i ,ret;
    while(1) {
        //usb有数据下来 用proc_cpu0;
        
        ret = OAL_sem_wait(&usb_r71);
        if ( ret == OAL_SUCCESS) {
            render_7_1_proc_stage_cpu0(r71, ch_in_cpu0, MIC_BASE_DATA_NUM);
            if(first_frame) {//第一帧
                first_frame = 0;
                for(i = 0; i < 8; i++) {
                    memcpy(ch_in_cpu1[i], ch_in_cpu0[i], sizeof(float)*MIC_BASE_DATA_NUM);
                }
                OAL_cpux_work_trigger(r71_cpux_handle, r71_cpu1_process, r71);
            }
            //调起CPU1去跑proc_cpu1
            if (r71_cpux_handle) {
                OAL_cpux_work_wait_finished(r71_cpux_handle);
            }
            usbplay_get_interleave_2ch_data(usba_proc_outbuffer, ch_in_cpu1, MIC_BASE_DATA_NUM);
            for(i = 0; i < 8; i++) {
                memcpy(ch_in_cpu1[i], ch_in_cpu0[i], sizeof(float)*MIC_BASE_DATA_NUM);
            }
            OAL_cpux_work_trigger(r71_cpux_handle, r71_cpu1_process, r71);
        }
    }
    return NULL;
}
#endif

AT_SRAM(.usbd_audio)
void on_usbd_down_stream(uint8_t _INOUT *newBuf, uint16_t _INOUT length)
{
    __maybe_unused uint16_t samples;
    if(!usbdev_ct->g_isPcmMute) {
#if USB_AUDIO_DOLBY_SUPPORT_EN
        if(usbd_get_audio_mode() == AUDIO_DOLBY) {
            //todo .....
            //TODO: 先推0数据，然后将数据分成8ch,告诉线程可以处理了
            samples = length / (8*sizeof(int16_t));
            usbplay_write(usba_proc_outbuffer, samples);
            usbplay_put_deinterleave_8ch_data((int16_t*)newBuf, ch_in_cpu0, samples);
            OAL_sem_post(&usb_r71);
        } else {
#endif
            usbplay_write(newBuf, length / (2*sizeof(int16_t)));
#if USB_AUDIO_DOLBY_SUPPORT_EN
        }
#endif
    }

    //g_pcmSampleRate 这个变量保存着采样率的值

    //g_pcmPlayBuffSize = 48*2*2;   //默认最大的size是这个值，如果总数据量不超过192BYTE，可以不用改这个变量的值
}


AT_SRAM(.usbd_audio)
void on_get_cur(USBD_COMP_AUDIO_ControlTypeDef* pControl, uint16_t* _OUT curValue)
{
    //有些手机set_cur完成后，会发get_cur来对比一下，
    //如果两个数据不对，虽说也能用，但是枚举过程会变长，它会反复发
    //音量设置的时候最为常见,所以应该修改 curValue  值为最新的逻辑值
    if(setupReq.wLength == 3) {  //返回采样率给主机
        usb_ep0buf[2] = g_pcmSampleRate >> 24;  //3BYTE采样率的高字节
        *curValue = g_pcmSampleRate & 0xFFFF;   //低的填到这里
    } else if(setupReq.wLength == 2) {
        //这里返回逻辑音量值
        if(HIBYTE(setupReq.wIndex) == 0x02) {
            *curValue = usbdev_ct->g_logicSpeakerVolume;
        } else if(HIBYTE(setupReq.wIndex) == 0x05) {
            //MIC的音量值
            logi("get mic volume %d", usbdev_ct->g_logicMicVolume);
            *curValue = usbdev_ct->g_logicMicVolume;
        }
    } else if(setupReq.wLength == 1){
        logi("get cur inde %x", setupReq.wIndex);
        if(HIBYTE(setupReq.wIndex) == 0x02) {
            *curValue = usbdev_ct->g_isPcmMute;
            logi("get cur mute %d", usbdev_ct->g_isPcmMute);
        } else if(HIBYTE(setupReq.wIndex) == 0x05) {
            //MIC的MUTE值
            *curValue = usbdev_ct->g_isMicMute;
            logi("get cur mute %d", usbdev_ct->g_isMicMute);
        }
    }
    logi("get cur %d", setupReq.wLength);
}

void hal_set_index(uint8_t index);
void usb_iso_transfer(uint8_t* buff,uint16_t size);

AT_SRAM(.usbd_audio)
void on_set_cur(USBD_COMP_AUDIO_ControlTypeDef* pControl)
{
    uint32_t pcmSampleRate_temp;
    //以下处理标准来讲应该按照音频命令以及描述符中的接口号，端点号等信息来处理
    //如果自定义或者其它的音频设备，这里处理要注意下
    logi("on_set_cur");
    //这里会进行采样率，音量等设置,简单粗暴用长度来判断
    //printf_array(&(pControl->setup_bak), 8);
    if(pControl->setup_bak.wLength == 3){    //设置采样命令
        if(((pControl->setup_bak.wIndex & 0x82) != 0x82) &&
            (pControl->setup_bak.wIndex & 0x02)) {//设置headset的采样率，这里02是描述符中对应接口号
            pcmSampleRate_temp = pControl->data[2];
            pcmSampleRate_temp <<= 8;
            pcmSampleRate_temp += pControl->data[1];
            pcmSampleRate_temp <<= 8;
            pcmSampleRate_temp += pControl->data[0];

            if((pcmSampleRate_temp!=g_pcmSampleRate) || !is_usbplay_ready()) {
                g_pcmSampleRate = pcmSampleRate_temp;
                logi("pcm sample rate change to %d", g_pcmSampleRate);
                event_force_unique_put(USB_EVENT_PLAY_PARAMS_CHANGE,
                    &g_pcmSampleRate, sizeof(g_pcmSampleRate));
            }
        } 
#if USB_MIC_NEED_RESAMPLE
        else if((pControl->setup_bak.wIndex & 0x82) == 0x82) {
            pcmSampleRate_temp = pControl->data[2];
            pcmSampleRate_temp <<= 8;
            pcmSampleRate_temp += pControl->data[1];
            pcmSampleRate_temp <<= 8;
            pcmSampleRate_temp += pControl->data[0];

            if(pcmSampleRate_temp != g_micSampleRate) {
                g_micSampleRate = pcmSampleRate_temp;

                //uint8_t bak = readw(0x40040012);
    
                void usbrec_samplerate_change(uint32_t sample_rate);
                usbrec_samplerate_change(pcmSampleRate_temp);
            }
            g_micUpstreamDataSize = pcmSampleRate_temp/1000 * 2 * MIC_INPUT_CHANNALS;

            hal_set_index(EP_ISOC);
            USB_FlushRxFifo(EP_ISOC);
            usb_iso_transfer((uint8_t*)ptr_mic_data_buff,g_micUpstreamDataSize);
            
            logi("microphone sample rate change %d, %d", g_micSampleRate, g_micUpstreamDataSize);
        }
#endif 
    } else if(pControl->setup_bak.wLength == 2) {
        //这里处理音量交互命令
        uint16_t vol = (uint16_t)pControl->data[0] +  ((uint16_t)pControl->data[1] << 8);
        if(HIBYTE(pControl->setup_bak.wIndex) == 0x02) {
            //PCM的音量
            logi("pcm volume change %d", (uint16_t)pControl->data[0] +  ((uint16_t)pControl->data[1] << 8) );
            if (vol >= MIN_LOGIG_VOLUME && vol <=MAX_LOGIG_VOLUME) {
                usbdev_ct->g_logicSpeakerVolume = vol;
                event_put(USB_EVENT_PCM_VOLUME, NULL, 0);
            }
        } else if(HIBYTE(pControl->setup_bak.wIndex) == 0x05) {
            logi("mic volume change %d", (uint16_t)pControl->data[0] +  ((uint16_t)pControl->data[1] << 8) );
            usbdev_ct->g_logicMicVolume = vol;
        }
    } else if (pControl->setup_bak.wLength == 1) {
        if(HIBYTE(pControl->setup_bak.wIndex) == 0x02){
            usbdev_ct->g_isPcmMute = pControl->data[0];
            event_put(USB_EVENT_PCM_VOLUME, NULL, 0);
            logi("pcm mute %d",usbdev_ct->g_isPcmMute);
        }else if(HIBYTE(pControl->setup_bak.wIndex) == 0x05) {
            usbdev_ct->g_isMicMute = pControl->data[0];
            logi("mic mute %d",usbdev_ct->g_isMicMute);
        }
    }
}

AT_SRAM(.usbd_audio)
void on_get_min(uint16_t* _OUT min)
{
    //最小音量逻辑值
    logi("get min, %d", setupReq.wLength);
    *min = MIN_LOGIG_VOLUME;
}

AT_SRAM(.usbd_audio)
void on_get_max(uint16_t* _OUT max)
{
    //最大音量逻辑值
    logi("get max, %d",setupReq.wLength);
    *max = MAX_LOGIG_VOLUME;
}

AT_SRAM(.usbd_audio)
void on_get_res(uint16_t* _OUT step)
{
    //音量的逻辑步进值
    *step = USB_VOLUME_STEP;
    logi("get res, %d",setupReq.wLength);
}

AT_SRAM(.usbd_audio)
void on_set_res(USBD_COMP_AUDIO_ControlTypeDef* _IN pControl)
{
    //主机设置的步进值，大部分场景下不用作处理
}

AT_SRAM(.usbd_audio)
void on_alternate_setting_change()
{
    //音频接口改变的通知，在停止播放/录音，或者开始播放/录音的时候会发一个这个命令
}

AT_SRAM(.usbd_audio)
BOOL ExternAudioCmd(USBD_SetupReqTypedef* _IN setupReq)
{
    //扩展命令，在音频0-4的命令之外的USB命令会进到这里来
    //如果这里的命令有数据处理,那么就会进入 ExternAudioData处理相关数据
    return TRUE;
}

AT_SRAM(.usbd_audio)
BOOL ExternAudioData(USBD_COMP_AUDIO_ControlTypeDef* _IN pControl)
{
    return TRUE;
}

void usbdev_volume_set_process(void)
{
    uint16_t vol = 0;
    if(!usbdev_ct->g_isPcmMute) {
        vol = usbdev_ct->g_logicSpeakerVolume;
    }
    am_usbplay_volume_set(vol);
}


void USBD_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len);
extern USBD_DescriptorsTypeDef COMP_Desc;
#define USBD_PRODUCT_STRING        "USB demo Device"
uint8_t USBD_StrDesc[256];

uint8_t* usbd_set_string_demo(uint16_t *length)
{
    //此函数返回一个string描述符
    USBD_GetString((uint8_t *)USBD_PRODUCT_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

void usbd_audio_app_init()
{
    logi("%s",__func__);
    usbdev_ct = libc_calloc(sizeof(usbdev_context_t));
    usbdev_ct->g_logicSpeakerVolume = 100;          //这里设置插入设备的初始音量，如手机插入后会以此为最大音量进行音量调整，可根据需求更改

    ptr_pcm_data_buff = OAL_malloc(SPK_BUFF_SIZE);  //768
    ptr_mic_data_buff = OAL_malloc(MIC_BUFF_SIZE);

    uint8_t usb_audion_mode = psm_usb_audio_mode_get();
    logi("usb_audion_mode.... %d", usb_audion_mode);
    usbd_set_audio_mode(usb_audion_mode);   //AUDIO_STEREO

#if USB_AUDIO_DOLBY_SUPPORT_EN    //TODO:初始化r7.1并创建个线程跑loop;创建完loop创建个cpu1任务
    if (usbd_get_audio_mode() == AUDIO_DOLBY) {
        r71 = render_7_1_init(g_pcmSampleRate, MIC_BASE_DATA_NUM);
        if (r71 == NULL) {
            logd("7_1 init is fail"); 
        } else {
            logd("7_1 init is sucessful"); 
            ch_in_cpu0[0] = &usba_proc_inbuffer[0];
            ch_in_cpu0[1] = &usba_proc_inbuffer[48];
            ch_in_cpu0[2] = &usba_proc_inbuffer[2*48];
            ch_in_cpu0[3] = &usba_proc_inbuffer[3*48];
            ch_in_cpu0[4] = &usba_proc_inbuffer[4*48];
            ch_in_cpu0[5] = &usba_proc_inbuffer[5*48];
            ch_in_cpu0[6] = &usba_proc_inbuffer[6*48];
            ch_in_cpu0[7] = &usba_proc_inbuffer[7*48];

            ch_in_cpu1[0] = &usba_proc_inbuffer[8*48];
            ch_in_cpu1[1] = &usba_proc_inbuffer[9*48];
            ch_in_cpu1[2] = &usba_proc_inbuffer[10*48];
            ch_in_cpu1[3] = &usba_proc_inbuffer[11*48];
            ch_in_cpu1[4] = &usba_proc_inbuffer[12*48];
            ch_in_cpu1[5] = &usba_proc_inbuffer[13*48];
            ch_in_cpu1[6] = &usba_proc_inbuffer[14*48];
            ch_in_cpu1[7] = &usba_proc_inbuffer[15*48];
            //创建个信号量或者全局变量作消息传递
            if(usb_r71 == NULL) {
                usb_r71 = OAL_sem_create_binary();
            }
            if(r71_task_handle != NULL) {
                r71_task_handle = OAL_thread_create(r71_loop, r71, "r71_task", 9, 256);
            }
            r71_cpu1_init();
        }
    }
#endif
    usbd_init_kernel_audio(&g_UsbdAudioCb);
    //在函数usbd_start调用前 重写 COMP_Desc 相关函数实现字符串的设置
    //COMP_Desc.GetProductStrDescriptor = usbd_set_string_demo;
#if USB_EQ_EN
    extern void usb_eq_init();
    usb_eq_init();
    g_compAudioCallback.UsbExtendCommand = usbExtendCommand;
    g_compAudioCallback.pInquiry = compAudioString;
    register_callback(&g_compAudioCallback);
#elif USB_AENC_TOOLS_EN
    extern void usb_aenc_bwe_init();
    usb_aenc_bwe_init();
    g_compAudioCallback.UsbExtendCommand = usbAnecbweExtendCommand;
    g_compAudioCallback.pInquiry = compAencAudioString;
    register_callback(&g_compAudioCallback);
#endif

    logi("usbd_start");
    usbd_start();
}

void usbd_audio_app_deinit()
{
    logi("pre stop");
    USB_Disconnect();
    logi("%s",__func__);
    OAL_free(ptr_pcm_data_buff);
    ptr_pcm_data_buff = NULL;

    OAL_free(ptr_mic_data_buff);
    ptr_mic_data_buff = NULL;

    usbd_deinit_kernel_audio();

    libc_free(usbdev_ct);
    usbdev_ct = NULL;
}

void usbd_change_mode(uint8_t mode);
void usbd_audio_config_swicth()
{
#if USBD_MODE == USB_MODE_TWO_AUDIO_CONFIG
    static uint8_t mode = USB_MODE_SPEAKER_MIC;
    OAL_timer_activate(&_pc_out_detect_timer, OAL_TIMER_NO_ACTIVATE);   //先停止检测定时器
    usbd_audio_app_deinit();
    //断开USB，延时一段时间，让主机确认设备已经移除
    HAL_msleep(500);
    
    mode = !mode;
    usbd_change_mode(mode);
    usbd_audio_app_init();
    OAL_timer_activate(&_pc_out_detect_timer, OAL_TIMER_AUTO_ACTIVATE);
#endif
}

#else
void audition_initialize();

void usbd_audio_app_init()
{
    audition_initialize();
}

void usbdev_volume_set_process(void)
{
}

#endif
