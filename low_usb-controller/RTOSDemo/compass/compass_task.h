#ifndef __COMPASS_TASK_H__
#define __COMPASS_TASK_H__

typedef struct _piostat
{
  unsigned int portchange;
} piostat;

typedef struct _compassstat
{
  char newdataready;
  unsigned char header;
  unsigned short x;
  unsigned short y;
  unsigned int data;
  unsigned int currentoffset;
} compassstat;

piostat iostat;
compassstat cmpstat;

void vStartCompassTask( unsigned portBASE_TYPE uxPriority );

#endif
