#ifndef __TEST_H__
#define __TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

unsigned int Battery_GetVoltage(int iADC);
unsigned char Battery_GetPresentage(unsigned int uiVoltage);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
