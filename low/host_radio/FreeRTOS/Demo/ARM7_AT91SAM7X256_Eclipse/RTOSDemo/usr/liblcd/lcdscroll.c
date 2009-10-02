#include "lcd.h"

void lcdscroll(char offset)
{
  WriteSpiCommand(VSCRDEF);
  WriteSpiData(50);
  WriteSpiData(28);
  WriteSpiData(50);
  Delaya(2000);

  WriteSpiCommand(SEP);
  WriteSpiData(offset);
  Delaya(2000);

  WriteSpiCommand(NORON);
  Delaya(2000);
}
