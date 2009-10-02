//#include "ioat91sam7x256.h"
#include "AT91SAM7X256.h"
#include "lcd.h"
//#include "bmp.h"
//#include "bits.h"

AT91PS_PIO    l_pPioA   = AT91C_BASE_PIOA;
AT91PS_PIO    l_pPioB   = AT91C_BASE_PIOB;
AT91PS_SPI    l_pSpi    = AT91C_BASE_SPI0;
AT91PS_PMC    l_pPMC    = AT91C_BASE_PMC;
AT91PS_PDC    l_pPDC    = AT91C_BASE_PDC_SPI0;


#define SPI_SR_TXEMPTY

#define LCD_RESET_LOW     l_pPioA->PIO_CODR   = BIT2
#define LCD_RESET_HIGH    l_pPioA->PIO_SODR   = BIT2

void Delaya (unsigned long a) { while (--a!=0); }

void Delay_ (unsigned long a) 
{
  volatile unsigned long d;
  d=a;
  while (--d!=0);
}

unsigned int i,j;

#define BIT2 1<<2
#define BIT12 1<<12
#define BIT16 1<<16
#define BIT17 1<<17
#define BIT18 1<<18
#define BIT20 1<<20

void InitLCD(void) {

  // Pin for backlight
  //l_pPioB->PIO_CODR   = BIT20;    // Set PB20 to LOW
  l_pPioB->PIO_SODR   = BIT20;    // Set PB20 to HIGH
  l_pPioB->PIO_OER    = BIT20;    // Configure PB20 as output

  // Reset pin
  l_pPioA->PIO_SODR   = BIT2;     // Set PA2 to HIGH
  l_pPioA->PIO_OER    = BIT2;     // Configure PA2 as output

  // CS pin
  //l_pPioA->PIO_SODR   = BIT12;     // Set PA2 to HIGH
  //l_pPioA->PIO_OER    = BIT12;     // Configure PA2 as output


  // Init SPI0
  //set functionality to pins:
  //port0.12 -> NPCS0
  //port0.16 -> MISO
  //port0.17 -> MOSI
  //port0.18 -> SPCK
  l_pPioA->PIO_PDR = BIT12 | BIT16 | BIT17 | BIT18;
  l_pPioA->PIO_ASR = BIT12 | BIT16 | BIT17 | BIT18;
  l_pPioA->PIO_BSR = 0;


  //enable the clock of SPI
  l_pPMC->PMC_PCER = 1 << AT91C_ID_SPI0;
  //l_pPMC->PMC_PCER = 1 << 5;

  // Fixed mode
  l_pSpi->SPI_CR      = 0x81;               //SPI Enable, Sowtware reset
  l_pSpi->SPI_CR      = 0x01;               //SPI Enable


  //l_pSpi->SPI_MR      = 0xE0099;           //Master mode, fixed select, disable decoder, FDIV=1 (NxMCK), PCS=1110, loopback
  //l_pSpi->SPI_MR      = 0xE0019;            //Master mode, fixed select, disable decoder, FDIV=1 (NxMCK), PCS=1110,
  //l_pSpi->SPI_MR      = 0xE0011;            //Master mode, fixed select, disable decoder, FDIV=0 (MCK), PCS=1110
  l_pSpi->SPI_MR      = 0xE0019;            //Master mode, fixed select, disable decoder, FDIV=1 (MCK), PCS=1110

  //l_pSpi->SPI_CSR[0]  = 0x4A02;             //8bit, CPOL=0, ClockPhase=1, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x4A13;             //9bit, CPOL=1, ClockPhase=1, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x4A12;             //9bit, CPOL=0, ClockPhase=1, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x4A11;             //9bit, CPOL=1, ClockPhase=0, SCLK = 200kHz
  //l_pSpi->SPI_CSR[0]  = 0x01011F11;           //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/32*31 = 48kHz
  // work l_pSpi->SPI_CSR[0]  = 0x01010F11;           //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/32*15 = 96kHz
  l_pSpi->SPI_CSR[0]  = 0x01010C11;           //9bit, CPOL=1, ClockPhase=0, SCLK = 48Mhz/32*12 = 125kHz

}


void WriteSpiCommand(unsigned int data){

  data = (data & ~0x0100);

  // Wait for the transfer to complete
  while((l_pSpi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);

  l_pSpi->SPI_TDR = data;
}

void WriteSpiData(unsigned int data){

  data = (data | 0x0100);

  // Wait for the transfer to complete
  while((l_pSpi->SPI_SR & AT91C_SPI_TXEMPTY) == 0);

  l_pSpi->SPI_TDR = data;
}


void Backlight(unsigned char state) {

  if(state == BKLGHT_LCD_ON)
    l_pPioB->PIO_SODR   = BIT20;    // Set PB20 to HIGH
  else
    l_pPioB->PIO_CODR   = BIT20;    // Set PB20 to LOW

}

void SetContrast(unsigned char contrast) {

#ifdef GE12
  WriteSpiCommand(CONTRAST);
  WriteSpiData(0x20+contrast);    // contrast
#else

#endif

}

void LCDSettings(void) {

#ifdef GE12

  // Hardware reset
  //LCD_RESET_LOW;
  //Delaya(2000);
  //LCD_RESET_HIGH;
  //Delaya(2000);

  ///// Initialization start /////

  // 2. Software Reset
  WriteSpiCommand(SOFTRST);
  Delaya(2000);

  // 3. Initial escape
  WriteSpiCommand(INITESC);
  Delaya(2000);

  ///// Initialization end /////

  ///// Display setting 1 /////

  // 1. Refresh set
  WriteSpiCommand(REFSET);
  WriteSpiData(0);

  // 2. Display control - 7 parameters
   /*
   WriteSpiCommand(DISPCTRL);
   WriteSpiData(128);     // Set the lenght of one selection term
   WriteSpiData(128);     // Set N inversion -> no N inversion
   WriteSpiData(129);     // Set frame frequence and bias rate -> 2 devision of frequency and 1/8 bias, 1/67 duty, 96x67 size
   //WriteByte(134, DATA);     // Set frame frequence and bias rate -> 2 devision of frequency and 1/8 bias, 1/67 duty, 96x67 size
   //WriteByte(0x8D, DATA);     // Set frame frequence and bias rate -> 2 devision of frequency and 1/8 bias, 1/67 duty, 96x67 size
   //WriteByte(132, DATA);     // Set frame frequence and bias rate -> 2 devision of frequency and 1/8 bias, 1/67 duty, 96x67 size
   WriteSpiData(84);      // Set duty parameter
   WriteSpiData(69);      // Set duty parameter
   WriteSpiData(82);      // Set duty parameter
   WriteSpiData(67);      // Set duty parameter
   */


  WriteSpiCommand(DISPCTRL);
  WriteSpiData(128);     // Set the lenght of one selection term
  WriteSpiData(128);     // Set N inversion -> no N inversion
  WriteSpiData(134);     // Set frame frequence and bias rate -> 2 devision of frequency and 1/8 bias, 1/67 duty, 96x67 size
  WriteSpiData(84);      // Set duty parameter
  WriteSpiData(69);      // Set duty parameter
  WriteSpiData(82);      // Set duty parameter
  WriteSpiData(67);      // Set duty parameter


  // 3.1 Grey scale 0 position set - 15 parameters
  WriteSpiCommand(GRAYSCALE0);
  WriteSpiData(1);     // GCP1 - gray lavel to be output when the RAM data is "0001"
  WriteSpiData(2);     // GCP2 - gray lavel to be output when the RAM data is "0010"
  WriteSpiData(4);     // GCP3 - gray lavel to be output when the RAM data is "0011"
  WriteSpiData(8);     // GCP4 - gray lavel to be output when the RAM data is "0100"
  WriteSpiData(16);    // GCP5 - gray lavel to be output when the RAM data is "0101"
  WriteSpiData(30);    // GCP6 - gray lavel to be output when the RAM data is "0110"
  WriteSpiData(40);    // GCP7 - gray lavel to be output when the RAM data is "0111"
  WriteSpiData(50);    // GCP8 - gray lavel to be output when the RAM data is "1000"
  WriteSpiData(60);    // GCP9 - gray lavel to be output when the RAM data is "1001"
  WriteSpiData(70);    // GCP10 - gray lavel to be output when the RAM data is "1010"
  WriteSpiData(80);    // GCP11 - gray lavel to be output when the RAM data is "1011"
  WriteSpiData(90);    // GCP12 - gray lavel to be output when the RAM data is "1100"
  WriteSpiData(100);   // GCP13 - gray lavel to be output when the RAM data is "1101"
  WriteSpiData(110);   // GCP14 - gray lavel to be output when the RAM data is "1110"
  WriteSpiData(127);   // GCP15 - gray lavel to be output when the RAM data is "1111"

  // 4. Gamma curve set - select gray scale - GRAYSCALE 0 or GREYSCALE 1
  WriteSpiCommand(GAMMA);
  WriteSpiData(1);     // Select grey scale 0


  // 5. Command driver output
  WriteSpiCommand(COMMONDRV);
  WriteSpiData(0);     // Set COM1-COM41 side come first, normal mod


  // 6. Set Normal mode (my)
  WriteSpiCommand(NORMALMODE);

  // 7. Inversion off
  // WriteSpiCommand(INVERSIONOFF);

  // 8. Column address set
  WriteSpiCommand(COLADDRSET);
  WriteSpiData(0);
  WriteSpiData(131);

  // 9. Page address set
  WriteSpiCommand(PAGEADDRSET);
  WriteSpiData(0);
  WriteSpiData(131);

  // 10. Memory access controler
  WriteSpiCommand(ACCESSCTRL);
  WriteSpiData(0x40);     // horizontal
  //WriteSpiData(0x20);   // vertical

  ///// Display setting 1 end  /////


  ///// Power supply  ///////

  // 1. Power control
  WriteSpiCommand(PWRCTRL);
  WriteSpiData(4);     // Internal resistance, V1OUT -> high power mode, oscilator devision rate

  // 2. Sleep out
  WriteSpiCommand(SLEEPOUT);


  // 3. Voltage control - voltage control and write contrast define LCD electronic volume
  WriteSpiCommand(VOLTCTRL);
  //WriteSpiData(0x7f);    //  full voltage control
  //WriteSpiData(0x03);    //  must be "1"

  // Memory access controler (command 0x36)
  WriteSpiCommand(MADCTL);
  WriteSpiData(0xC8);               // 0xC0 = mirror x and y, reverse rgb


  // 4. Write contrast
  WriteSpiCommand(CONTRAST);
  WriteSpiData(0x3b);    // contrast

  Delaya(2000);

  // 5. Temperature gradient
  WriteSpiCommand(TEMPGRADIENT);
  for(i=0; i<14; i++) {
    WriteSpiData(0);
  }

  // 6. Booster voltage ON
  WriteSpiCommand(BOOSTVON);


  // Finally - Display On
  WriteSpiCommand(DISPLAYON);	

#else


  // Hardware reset
  LCD_RESET_LOW;
  Delay_(1000);
  LCD_RESET_HIGH;
  Delay_(1000);

  // Display vontrol
  WriteSpiCommand(DISCTL);
//  WriteSpiData(0x03); // no division
//  WriteSpiData(0x23); // 160 line
//  WriteSpiData(0x02); // 2 highlighte line
  WriteSpiData(0x00); // default
  WriteSpiData(0x20); // (32 + 1) * 4 = 132 lines (of which 130 are visible)
  WriteSpiData(0x0a); // default

  // COM scan
  WriteSpiCommand(COMSCN);
  WriteSpiData(0x00);  // Scan 1-80

  // Internal oscilator ON
  WriteSpiCommand(OSCON);

  // wait aproximetly 100ms
  Delay_(10000);

  // Sleep out
  WriteSpiCommand(SLPOUT);

  // Voltage control
  WriteSpiCommand(VOLCTR);
  WriteSpiData(0x1F); // middle value of V1
  WriteSpiData(0x03); // middle value of resistance value

  // Temperature gradient
  WriteSpiCommand(TMPGRD);
  WriteSpiData(0x00); // default

  // Power control
  WriteSpiCommand(PWRCTR);
  WriteSpiData(0x0f);   // referance voltage regulator on, circuit voltage follower on, BOOST ON

  // Normal display
  WriteSpiCommand(DISNOR);

  // Inverse display
  WriteSpiCommand(DISINV);

  // Partial area off
  WriteSpiCommand(PTLOUT);

//  // Scroll area set
//  WriteSpiCommand(ASCSET);
//  WriteSpiData(0);
//  WriteSpiData(0);
//  WriteSpiData(40);
//  WriteSpiData(3);

//  // Vertical scrool address start
//  WriteSpiCommand(SCSTART);
//  WriteSpiData(0);


  // Data control
  WriteSpiCommand(DATCTL);
  WriteSpiData(0x00); // all inversions off, column direction
  WriteSpiData(0x03); // RGB sequence
  WriteSpiData(0x02); // Grayscale -> 16

  // Page Address set
  WriteSpiCommand(PASET);
  WriteSpiData(0);
  WriteSpiData(131);

  // Page Column set
  WriteSpiCommand(CASET);
  WriteSpiData(0);
  WriteSpiData(131);

#endif



}

/*void LCDWrite130x130bmp(void) {

#ifdef GE12

  // Display OFF
  WriteSpiCommand(DISPLAYOFF);

    // WRITE MEMORY
  WriteSpiCommand(MEMWRITE);


  for(j=0; j<sizeof(bmp); j++) {
    WriteSpiData(bmp[j]);
  }


  // Display On
  WriteSpiCommand(DISPLAYON);	

#else

  // Display OFF
  // WriteSpiCommand(DISOFF);

  // WRITE MEMORY
  WriteSpiCommand(RAMWR);

  //for(j=0; j<sizeof(bmp)-396; j++) {
  for(j=0; j<25740; j++) {
    WriteSpiData(bmp[j]);
    //WriteSpiData(0xFF);
  }

  // wait aproximetly 100ms
  Delay_(10000);

  // Display On
  WriteSpiCommand(DISON);	

#endif

}*/

void LCDclearbg(int top, int bottom, int left, int right,unsigned int color) 
{

#ifdef GE12

  // Display OFF
  //WriteSpiCommand(DISPLAYOFF);

  // WRITE MEMORY
  WriteSpiCommand(PASET);
  WriteSpiData(bottom);
  WriteSpiData(top);

  // Column address set (command 0x2A)
  WriteSpiCommand(CASET);
  WriteSpiData(left);
  WriteSpiData(right);
 
  WriteSpiCommand(MEMWRITE);

  int i = 0;

  for (i = 0; i < ((((top - bottom + 1) * (right - left + 1)) / 2) + 1); i++) 
    {
      // use the color value to output three data bytes covering two pixels
      WriteSpiData((color >> 4) & 0xFF);
      WriteSpiData(((color & 0xF) << 4) | ((color >> 8) & 0xF));
      WriteSpiData(color & 0xFF);
    }
  
  // Display On
  //WriteSpiCommand(DISPLAYON);	

#else

  // Display OFF
  // WriteSpiCommand(DISOFF);

  // WRITE MEMORY
  WriteSpiCommand(RAMWR);

  //for(j=0; j<sizeof(bmp)-396; j++) {
  for(j=0; j<25740; j++) {
    WriteSpiData(bmp[j]);
    //WriteSpiData(0xFF);
  }

  // wait aproximetly 100ms
  Delay_(10000);

  // Display On
  WriteSpiCommand(DISON);	

#endif

}
