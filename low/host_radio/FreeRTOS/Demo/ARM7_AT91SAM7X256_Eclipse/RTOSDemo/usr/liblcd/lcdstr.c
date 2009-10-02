// *************************************************************************************************
//                                            LCDPutStr.c
//
//     Draws a null-terminates character string at the specified (x,y) address, size and color
//
//        Inputs:        pString =    pointer to character string to be displayed
//                       x       =    row address (0 .. 131)
//                       y       =    column address (0 .. 131)
//                       Size    =    font pitch (SMALL, MEDIUM, LARGE)
//                       fColor =     12-bit foreground color value   rrrrggggbbbb
//                       bColor =     12-bit background color value   rrrrggggbbbb
//
//
//        Returns:    nothing
//
//     Notes: Here's an example to display "Hello World!" at address (20,20)
//
//              LCDPutChar("Hello World!", 20, 20, LARGE, WHITE, BLACK);
//
//
//     Author: James P Lynch       July 7, 2007
// *************************************************************************************************


#include "lcd.h"

void LCDPutStr(char *pString, int x, int y, int Size, int fColor, int bColor) {
       // loop until null-terminator is seen
       while (*pString != 0x00) {
               // draw the character
               LCDPutChar(*pString++, x, y, Size, fColor, bColor);
               // advance the y position
               if (Size == SMALL)
                       y = y + 6;
               else if (Size == MEDIUM)
                       y = y + 8;
               else
                       y = y + 8;
               // bail out if y exceeds 131
               if (y > 131) break;
       }
}
// *****************************************************************************
//                                            Delay.c
//
//             Simple for loop delay
//
//             Inputs: a - loop count
//
//             Author: James P Lynch June 27, 2007
// *****************************************************************************
void Delay (unsigned long a) {
       while (--a!=0);
}

