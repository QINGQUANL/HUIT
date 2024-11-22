
#ifndef _MIXER_H_
#define _MIXER_H_

int HAL_mixer_set_ctl(uint32_t id, const uint32_t ctl, const uint32_t val);
int HAL_mixer_get_ctl(uint32_t id, uint32_t ctl, uint32_t *val);

#endif /* _MIXER_H_ */
