#ifndef _WAKEUP_H_
#define _WAKEUP_H_

#define HDRCNT	32
#define BUFLEN	256

#ifdef __cplusplus
extern "C"{
#endif

void WakeupInit(void);
void WakeupRelease(void);
float OnWaveData(short* data, int len/* = BUFLEN*/);//return -1.0=null; others=hisiri confidence;
float OnFileData(short* data, int len/* = HDRCNT*BUFLEN*/);//return -1.0=null; others=hisiri confidence;

int OnWaveDataPrepare(short* data, int len/* = BUFLEN*/);
void OnWaveDataProc1(void);
void OnWaveDataProc2Prepare(void);
float OnWaveDataProc2(void);
void OnWaveDataClear(void);

#ifdef __cplusplus
}
#endif

#endif