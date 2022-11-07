#ifndef _STATISTIC_H_
#define _STATISTIC_H_

#define BOOT_RESULT_FLAG_RF_ERROR  0x01

extern unsigned char ucBootResultFlags;

void Statistic_Init();
void Statistic_ButtonActivate();
void Statistic_1HzHook();

#endif
