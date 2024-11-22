#ifndef _AUTHORITY_H_
#define _AUTHORITY_H_

void set_certification_code(const uint8_t* certCode);
bool get_certification_check();
void vf_table_auth(void);
void rf_table_auth(short *freq_int, int *freq_frac);

#endif