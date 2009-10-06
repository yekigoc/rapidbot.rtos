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
  AT91C_BASE_SPI->SPI_MR      = 0x90E0011;//0xE0019;
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
  AT91C_BASE_SPI->SPI_CSR[0]  = 0x109FF80;//0x01010F11;//0x01010C11;           //9bit, CPOL=1,
						      //ClockPhase=0,
						      //SCLK =
						      //48Mhz/32*12 =
						      //125kHz
  //  AT91C_BASE_SPI->SPI_CR      = 0x01;               //SPI Enable
}

void tr24_initframer()
{
  unsigned int data = 0x30;	    
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 17;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x9800;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 12;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x31;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 1;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0xFF8F;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 2;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x32;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 3;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x8028;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 4;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x33;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 5;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x8056;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 6;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x34;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 7;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x4EF6;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 8;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x35;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 9;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x185C;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 10;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x36;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 19;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x185C;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 20;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x37;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 21;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0xD651;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 22;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x38;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 23;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x4444;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 24;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0x39;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 25;

  UTIL_WaitTimeInUs(BOARD_MCK, 100);
	    
  data = 0xE000;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 26;
}

void tr24_initrfic()
{
  unsigned int data = 0x09;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 30;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x2001;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 31;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x00;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 32;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x354D;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 33;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  data = 0x02;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 34;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x1F01;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 35;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x04;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 36;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0xBCF0;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 37;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x05;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 38;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x00A1;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 37;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x07;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 38;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x124C;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 39;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x08;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 40;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x8000;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 41;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x0C;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 42;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x8000;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 43;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x0E;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 44;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x169B;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 45;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x0F;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 46;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x90AD;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 47;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x10;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 48;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0xB000;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 49;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x13;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 50;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0xA114;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 51;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x14;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 52;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x8191;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 53;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x16;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 54;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x0002;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 55;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x18;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 56;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0xB140;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 57;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x19;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 58;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0xA80F;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 69;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x1A;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 70;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x3F04;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 71;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);

  data = 0x1C;
  // Wait for the transfer to complete
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 72;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
  
  data = 0x5800;
  while((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0);
  AT91C_BASE_SPI->SPI_TDR = data;
  trspistat.trinited = 73;
  
  UTIL_WaitTimeInUs(BOARD_MCK, 100);
}
