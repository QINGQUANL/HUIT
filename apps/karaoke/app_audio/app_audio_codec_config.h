#ifndef _APP_AUDIO_CODEC_CONFIG_H__
#define _APP_AUDIO_CODEC_CONFIG_H__

void app_audio_codec_analog_gain_config(void);
void app_audio_codec_config(void);
uint32_t codec_adc_input_source_get(uint8_t mode);
uint8_t audio_rec_input_sel_get(void);
void audio_rec_input_sel_set(uint8_t sel);
uint8_t audio_linein_mode_input_sel_get(void);
void audio_linein_mode_input_sel_set(uint8_t sel);


void app_audio_hw_init(void);
#endif

