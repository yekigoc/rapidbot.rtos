#ifndef __COMMON_STRUCTS_H__
#define __COMMON_STRUCTS_H__

#define USB_PWM_W    0x02
#define USB_ADC_R    0x05
#define USB_TIME_R   0x03

typedef struct _pwmparams
{
  unsigned short dutycycles[2];
  unsigned char cyclechange;
} pwmparams;

typedef struct _compassparams
{
  unsigned char compassstat;
  unsigned int compassdata;
} compassparams;

#endif //__COMMON_STRUCTS_H__
