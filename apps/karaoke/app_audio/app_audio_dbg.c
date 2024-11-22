#include "pcm.h"
#include "hal_snd_hardware.h"
#include "app_audio_dbg.h"
#include "sltk_math.h"

/*
    该debug接口位于pcm_read和pcm_write
    可用于检查dma数据 搬/运 之 后/前 是否有问题
    具体用法：
        根据info中的
        ch           : 确定通道数
        samples_size : 确定采样精度
        id           ：确定设备
        direction    ：确定输入还是输出
        针对性的进行debug
*/
AT_SRAM(.app_adbg)
static int app_audio_debug_proc(pcm_params_info* info, const void* buf, int samples)
{
#if 0
    if(info && buf) {
        if(info->ch == 2 &&
            info->sample_size == 4 &&
            info->direction == PCM_OUT &&
            info->id == SND_CODEC) {
            gen_1Khz((int32_t*)buf, 2, samples);
        }
    }
#endif
    return 0;
}

void app_audio_debug_init()
{
    pcm_dbg_hook* dbg_addr = pcm_export_dbg_interface();
    *dbg_addr = (pcm_dbg_hook)app_audio_debug_proc;
}