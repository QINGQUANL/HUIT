#ifndef _CPU_FREQ_H_
#define _CPU_FREQ_H_

#define CPU_CORE_NUM	2

typedef struct {
    uint32_t amixer_freq[CPU_CORE_NUM];
    uint32_t effects_freq[CPU_CORE_NUM];
    uint32_t tone_mixer_freq[CPU_CORE_NUM];
    uint32_t a2dp_audio_freq[CPU_CORE_NUM];
    uint32_t sco_audio_freq[CPU_CORE_NUM];
    uint32_t sco_anec_freq[CPU_CORE_NUM];
    uint32_t usb_audio_freq[CPU_CORE_NUM];
    uint32_t linein_audio_freq[CPU_CORE_NUM];
    uint32_t encode_wav_freq[CPU_CORE_NUM];
    uint32_t encode_mp3_freq[CPU_CORE_NUM];
    uint32_t decode_wav_freq[CPU_CORE_NUM];
    uint32_t decode_mp3_freq[CPU_CORE_NUM];
    uint32_t default_freq[CPU_CORE_NUM];
}cpu_freq_table;

void cpufreq_init_table(cpu_freq_table *tab);
#endif
