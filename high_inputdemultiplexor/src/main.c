#include <rgi-inputdemultiplexor.h>
#include <rgi-usbscheduler.h>

int main(void)
{
  g_type_init ();
  g_thread_init (NULL);

  GAsyncQueue* fromusb = g_async_queue_new();
  GAsyncQueue* tousb = g_async_queue_new();

  RgiUsbScheduler * scheduler = rgi_usbscheduler_new();
  scheduler->fromusb = fromusb;
  scheduler->tousb = tousb;

  RgiStateParam * p = RGI_STATEPARAM(rgi_adcparam_new());
  rgi_usbscheduler_pushparam(scheduler, p);
  p = RGI_STATEPARAM(rgi_pwmparam_new());
  rgi_usbscheduler_pushparam(scheduler, p);
  p = RGI_STATEPARAM(rgi_timeparam_new());
  rgi_usbscheduler_pushparam(scheduler, p);
  
  if (rgi_usbscheduler_start(scheduler)<0)
    return -1;
  RgiInputDemultiplexor * demux = rgi_inputdemultiplexor_new();
  demux->scheduler = scheduler;
  demux->fromusb = fromusb;
  demux->tousb = tousb;
  rgi_inputdemultiplexor_start(demux);
  g_async_queue_unref (demux->fromusb);
  g_async_queue_unref (demux->tousb);

  return 0;
}
