#include "libterm.h"

void term_init(term_stat * ts)
{
  char i;
  for (i=0; i<16; i=i+1)
    {
      ts->lines[i]=(void*)0;
      ts->index[i]=i;
    }
}

void term_print(term_stat * ts)
{
  char i=0;
  char j=0;
  char b = 0;
  for (i=0; i<16; i=i+1)
    {
      for (j=0; j<16; j=j+1)
	{
	  if (ts->index[j]==i)
	    {
	      if (ts->lines[j] != (void*)0)
		{
		  LCDclearbg(b+8, b, 0, 131, WHITE);
		  LCDPutStr(ts->lines[j], b, 5, SMALL, BLUE, WHITE);
		}
	      b=b+8;  
	    }
	}
    }
}

void term_pushstr(term_stat * ts, char * str)
{
  char i;
  for (i=0;i<16;i=i+1)
    {
      ts->index[i]=ts->index[i]+1;
      if (ts->index[i]==17)
	{
	  ts->index[i]=0;
	  ts->lines[i]=str;
	}
    }
}
