#include "Battery.h"

// Potentiometer: 560K and 330K
const unsigned int _puiBatteryPresentageTable[] =
{ 1190, 1194, 1198, 1204, 1212, 1220, 1228, 1236, 1246, 1258, 1270 };
// 0%,  10%,  20%,  30%,  40%,  50%,  60%,  70%,  80%,  90%,  100%

unsigned int Battery_GetVoltage(int iADC)
{
    long lTemp = iADC;
    return (int)(lTemp*5*100*(56+33)/33/1024);
}

unsigned char Battery_GetPresentage(unsigned int uiVoltage)
{
  unsigned char i, j;

  if (uiVoltage >= _puiBatteryPresentageTable[10])
      return 100;
  if (uiVoltage <= _puiBatteryPresentageTable[0])
      return 0;
  for (i = 10; i; i--)
  {
      if (uiVoltage > _puiBatteryPresentageTable[i-1])
      {
          j = (_puiBatteryPresentageTable[i] - _puiBatteryPresentageTable[i-1]);
          j = ((uiVoltage - _puiBatteryPresentageTable[i-1])*10 + (j>>1)) / j + (10*(i-1));
          if (j > 100)
              j = 100;
          return j;
      }
  }
  return 255;
}


