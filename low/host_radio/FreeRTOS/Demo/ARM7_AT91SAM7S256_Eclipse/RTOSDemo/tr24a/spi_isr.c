#include "tr24a.h"

//------------------------------------------------------------------------------
/// SPI0 interrupt handler. Forwards the event to the SPI driver handler.
//------------------------------------------------------------------------------
void ISR_Spi0(void)
{
  trspistat.spiisrcalls = trspistat.spiisrcalls + 1;
  //    SDSPI_Handler(&sdSpiDrv);
}
