#include "locator.h"
#include "FreeRTOS.h"

unsigned int ConvHex2mV( unsigned int valueToConvert )
{
  return( (ADC_VREF * valueToConvert)/0x3FF);
}

unsigned short adchanfindmax(unsigned char chan)
{
  int i = 0;
  unsigned short max = 0;
  for (i=0;i<256;i++)
    {
      if (trspistat.channels[chan].adcbuf[i]-MIDDLEPOINT>max)
	max = trspistat.channels[chan].adcbuf[i]-MIDDLEPOINT;
    }
  return max;
}
