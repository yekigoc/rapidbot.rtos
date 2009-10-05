#include "tr24a.h"
#include "FreeRTOS.h"
#include "pio/pio.h"
#include "spi/spi.h"

static const Pin pins[] = 
{
  TRRESET_PIO,
  MISO_PIO,
  MOSI_PIO,
  NPCS_PIO,
  SPCK_PIO,
  FIFOLG_PIO,
  PKTFLG_PIO
};

//------------------------------------------------------------------------------
/// Wait time in ms
//------------------------------------------------------------------------------
void UTIL_Loop(unsigned int loop)
{
    while(loop--);	
}


void UTIL_WaitTimeInMs(unsigned int mck, unsigned int time_ms)
{
    register unsigned int i = 0;
    i = (mck / 1000) * time_ms;
    i = i / 3;
    UTIL_Loop(i);
}

//------------------------------------------------------------------------------
/// Wait time in us
//------------------------------------------------------------------------------
void UTIL_WaitTimeInUs(unsigned int mck, unsigned int time_us)
{
    volatile unsigned int i = 0;
    i = (mck / 1000000) * time_us;
    i = i / 3;
    UTIL_Loop(i);
}

void tr24_init()
{
  PIO_Configure(pins, PIO_LISTSIZE(pins));
    
  //  AIC_ConfigureIT(AT91C_ID_SPI,  AT91C_AIC_PRIOR_LOWEST, ISR_Spi0);
  //SPI_Configure(&trspi, TR24_NPCS, 0xE0019);//AT91C_SPI_MSTR | AT91C_SPI_MODFDIS  | AT91C_SPI_PCS);
  //  AIC_EnableIT(AT91C_ID_SPI);
  
  /*  PIO_Clear(RESET_PIN);
      UTIL_WaitTimeInMs(BOARD_MCK, 2000);*/

  //  SPI_Enable(&trspi);

  //  AT91C_BASE_PMC->PMC_PCDR = 1 << AT91C_ID_SPI;
  //  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SPI;
  
  //  SPI_ConfigureNPCS(&trspi, TR24_NPCS, 0x01011F11);//SPI_SCBR(TR24_BAUDRATE, BOARD_MCK));
  AT91C_BASE_SPI->SPI_CR      = 0x81;               //SPI Enable, Sowtware reset
  AT91C_BASE_SPI->SPI_CR      = 0x01;               //SPI Enable


  //l_pSpi->SPI_MR      = 0xE0099;           //Master mode, fixed select, disable decoder, FDIV=1 (NxMCK), PCS=1110, loopback
  //l_pSpi->SPI_MR      = 0xE0019;            //Master mode, fixed select, disable decoder, FDIV=1 (NxMCK), PCS=1110,
  //l_pSpi->SPI_MR      = 0xE0011;            //Master mode, fixed select, disable decoder, FDIV=0 (MCK), PCS=1110
  AT91C_BASE_SPI->SPI_MR      = 0xE0019;            //Master mode, fixed select, disable decoder, FDIV=1 (MCK), PCS=1110

  //l_pSpi->SPI_CSR[0]  = 0x4A02;             //8bit, CPOL=0, ClockPhase=1, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x4A13;             //9bit, CPOL=1, ClockPhase=1, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x4A12;             //9bit, CPOL=0, ClockPhase=1, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x4A11;             //9bit, CPOL=1, ClockPhase=0, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x01011F11;           //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/32*31 = 48kHz
  // work l_pSpi->SPI_CSR[0]  = 0x01010F11;           //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/32*15 = 96kHz
  AT91C_BASE_SPI->SPI_CSR[0]  = 0x01010C11;           //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/32*12 = 125kHz

//  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SPI;
}
