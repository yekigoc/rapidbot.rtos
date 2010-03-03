#include <rgi-inputdemultiplexor.h>

int main(void)
{
  g_type_init ();
  g_thread_init (NULL);
  RgiInputDemultiplexor * demux = rgi_inputdemultiplexor_new();
  return 0;
}
