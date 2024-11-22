#ifndef _WAKEUP_H_
#define _WAKEUP_H_

#define HDRCNT	32
#define BUFLEN	256
#define SCALE	4096

#ifdef __cplusplus
extern "C"{
#endif

void WakeupInit(void);
void WakeupRelease(void);
int OnWaveData(short* data, int len/* = BUFLEN*/);//return -1.0=null; others=hisiri confidence; 
int OnFileData(short* data, int len/* = HDRCNT*BUFLEN*/);//return -1.0=null; others=hisiri confidence; 

#ifdef __cplusplus
}
#endif

#endif