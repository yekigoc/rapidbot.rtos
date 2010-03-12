#ifndef __COMMON_STRUCTS_H__
#define __COMMON_STRUCTS_H__

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
