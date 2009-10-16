#include "tr24a.h"

#ifdef AT91SAM7S
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
  FIFOFLG_PIO,
  PKTFLG_PIO
};
#endif

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
    i = i / 4;
    UTIL_Loop(i);
}

//------------------------------------------------------------------------------
/// Wait time in us
//------------------------------------------------------------------------------
void UTIL_WaitTimeInUs(unsigned int mck, unsigned int time_us)
{
    volatile unsigned int i = 0;
    i = (mck / 1000000) * time_us;
    i = i / 4;
    UTIL_Loop(i);
}

#ifdef AT91SAM7S
void tr24_init()
{
  PIO_Configure(pins, PIO_LISTSIZE(pins));
    
  //  AIC_ConfigureIT(AT91C_ID_SPI,  AT91C_AIC_PRIOR_LOWEST, ISR_Spi0);
  //  SPI_Configure(&trspi, TR24_NPCS, AT91C_SPI_MSTR | AT91C_SPI_MODFDIS  | AT91C_SPI_PCS);
  //  AIC_EnableIT(AT91C_ID_SPI);
  
  /*  PIO_Clear(RESET_PIN);
      UTIL_WaitTimeInMs(BOARD_MCK, 2000);*/

  //  SPI_Enable(&trspi);

  //  AT91C_BASE_PMC->PMC_PCDR = 1 << AT91C_ID_SPI;
  //  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SPI;
  
  //  SPI_ConfigureNPCS(&trspi, TR24_NPCS, SPI_SCBR(TR24_BAUDRATE,
  //  BOARD_MCK));
  AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_SPI;

  AT91C_BASE_SPI->SPI_CR      = 0x02;               //SPI Disable
  AT91C_BASE_SPI->SPI_CR      = 0x81;               //SPI Enable, Software reset
  AT91C_BASE_SPI->SPI_CR      = 0x81;               //SPI Enable,
						    //Software reset
  //l_pSpi->SPI_MR      = 0xE0099;           //Master mode, fixed select, disable decoder, FDIV=1 (NxMCK), PCS=1110, loopback
  //l_pSpi->SPI_MR      = 0xE0019;            //Master mode, fixed select, disable decoder, FDIV=1 (NxMCK), PCS=1110,
  //l_pSpi->SPI_MR      = 0xE0011;            //Master mode, fixed select, disable decoder, FDIV=0 (MCK), PCS=1110
  AT91C_BASE_SPI->SPI_MR      = 0x90E0011;//0x90E0011;//0xE0019;
					  ////Master mode, fixed
					  //select, disable decoder,
					  //FDIV=1 (MCK), PCS=1110
  AT91C_BASE_SPI->SPI_CR      = 0x02;               //SPI Disable
  AT91C_BASE_SPI->SPI_CR      = 0x01;               //SPI Enable
  //l_pSpi->SPI_CSR[0]  = 0x4A02;             //8bit, CPOL=0, ClockPhase=1, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x4A13;             //9bit, CPOL=1, ClockPhase=1, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x4A12;             //9bit, CPOL=0, ClockPhase=1, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x4A11;             //9bit, CPOL=1, ClockPhase=0, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x01011F11;           //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/32*31 = 48kHz
  // work l_pSpi->SPI_CSR[0]  = 0x01010F11;           //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/32*15 = 96kHz
  AT91C_BASE_SPI->SPI_CSR[0]  = 0x109FF00;//0x01010F11;//0x01010C11;           //9bit, CPOL=1,
						      //ClockPhase=0,
						      //SCLK =
						      //48Mhz/32*12 =
						      //125kHz
  //  AT91C_BASE_SPI->SPI_CR      = 0x01;               //SPI Enable
}
#endif

void tr24_writebyte(unsigned short byte)
{
#ifdef AT91SAM7S
  unsigned short data = byte;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data | SPI_PCS(1);
  while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TDRE) == 0);
#endif

#ifdef LPC2103
  unsigned char lodata = byte;
  SPI_SPDR=lodata;
  /* Wait for transfer to be completed */
  while(!(SPI_SPSR & SPIF));
#endif

  UTIL_WaitTimeInUs(BOARD_MCK, 10);
}

void tr24_writereg(unsigned short reg, unsigned short h, unsigned short l)
{
  /*  Pin ss = NPCS_PIO;

  PIO_Clear(&ss);

  UTIL_WaitTimeInUs(BOARD_MCK, 1);*/
#ifdef AT91SAM7S  
  unsigned short data = reg;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data | SPI_PCS(1);
  while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TDRE) == 0)

  UTIL_WaitTimeInUs(BOARD_MCK, 10);

  data = h;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data | SPI_PCS(1);
  while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TDRE) == 0)

  UTIL_WaitTimeInUs(BOARD_MCK, 10);

  data = l;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data | SPI_PCS(1);
  while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TDRE) == 0)

  UTIL_WaitTimeInUs(BOARD_MCK, 10);
#endif
#ifdef LPC2103
  unsigned char lodata = reg;
  SPI_SPDR=lodata;
  while(!(SPI_SPSR & SPIF));

  UTIL_WaitTimeInUs(BOARD_MCK, 10);

  lodata = h;
  SPI_SPDR=lodata;
  while(!(SPI_SPSR & SPIF));
  
  UTIL_WaitTimeInUs(BOARD_MCK, 10);

  lodata = l;
  SPI_SPDR=lodata;
  while(!(SPI_SPSR & SPIF));
#endif
  /*  PIO_Set(&ss);

      UTIL_WaitTimeInUs(BOARD_MCK, 1);*/
}

void tr24_initframer()
{
  tr24_writereg(0x30, 0x98, 0x00);
  tr24_writereg(0x31, 0xFF, 0x8F);
  tr24_writereg(0x32, 0x80, 0x28);
  tr24_writereg(0x33, 0x80, 0x56);
  tr24_writereg(0x34, 0x4E, 0xF6);
  tr24_writereg(0x35, 0xF6, 0xF5);
  tr24_writereg(0x36, 0x18, 0x5C);
  tr24_writereg(0x37, 0xD6, 0x51);
  tr24_writereg(0x38, 0x44, 0x44);
  tr24_writereg(0x39, 0xE0, 0x00);

#ifdef AT91SAM7S
  trspistat.trinited = 1001;
#endif
}

void tr24_initrfic()
{
  tr24_writereg(0x09, 0x20, 0x01);
  tr24_writereg(0x00, 0x35, 0x4D);
  tr24_writereg(0x02, 0x1F, 0x01);
  tr24_writereg(0x04, 0xBC, 0xF0);
  tr24_writereg(0x05, 0x00, 0xA1);
  tr24_writereg(0x07, 0x12, 0x4C);
  tr24_writereg(0x08, 0x80, 0x00);
  tr24_writereg(0x0C, 0x80, 0x00);
  tr24_writereg(0x0E, 0x16, 0x9B);
  tr24_writereg(0x0F, 0x90, 0xAD);
  tr24_writereg(0x10, 0xB0, 0x00);
  tr24_writereg(0x13, 0xA1, 0x14);
  tr24_writereg(0x14, 0x81, 0x91);
  tr24_writereg(0x16, 0x00, 0x02);
  tr24_writereg(0x18, 0xB1, 0x40);
  tr24_writereg(0x19, 0xA8, 0x0F);
  tr24_writereg(0x1A, 0x3F, 0x04);
  tr24_writereg(0x1C, 0x58, 0x00);
#ifdef AT91SAM7S
  trspistat.trinited = 1002;
#endif
}

void tr24_writefifo(char * msg, int len)
{
#ifdef AT91SAM7S
  Pin pkt = PKTFLG_PIO;
  Pin fifo = FIFOFLG_PIO;
  Pin ss = NPCS_PIO;
#endif
  int a = 0;

  a=a+1;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 10);

  tr24_writereg(0x07, 0x00, 0x80);  //Set DBUS_TX_EN (Enable
				    //transmission)
  //  memcpy(FIFObuf,msg,len ); // copy message to FIFObuf

  UTIL_WaitTimeInUs(BOARD_MCK, 10);

  tr24_writereg(0x52, 0x80, 0x00); //RESET TX FIFO

  UTIL_WaitTimeInUs(BOARD_MCK, 1);

  //  PIO_Clear(&ss);
  
  tr24_writebyte(0x50); //RESET TX

  UTIL_WaitTimeInUs(BOARD_MCK, 10);

  tr24_writebyte(0x02);

  tr24_writebyte(0x00);
  tr24_writebyte(0x00);

  UTIL_WaitTimeInUs(BOARD_MCK, 1);

  //  PIO_Set(&ss);

  UTIL_WaitTimeInUs(BOARD_MCK, 1);

  tr24_writereg(0x07, 0x00, 0x80);

  unsigned int spsr = SPI_SPSR;

  unsigned int i = 0;
  
#ifdef LPC2103
  while (!((IOPIN & PIN_FIFOFLG) || (IOPIN & PIN_PKTFLG)) && (i<BOARD_MCK))
    {
      i=i+1;
    }
#endif

#ifdef AT91SAM7S
  trspistat.trinited = 1003;

  while (!PIO_Get(&pkt))
    {
      trspistat.trinited = PIO_Get(&pkt)+PIO_Get(&fifo);
    }
  
  trspistat.trinited = 1100;  
#endif
}
