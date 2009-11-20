#include "./pwm.h"

//------------------------------------------------------------------------------
/// Interrupt handler for the PWM controller. Counts the number of periods and
/// updates the duty cycle after a given number.
//------------------------------------------------------------------------------

void ISR_Pwmc(void)
{
  static unsigned int count = 0;
  static unsigned int duty = MIN_DUTY_CYCLE;
  static unsigned char fadeIn = 1;
  
  // Interrupt on channel #1
  //  if ((AT91C_BASE_PWMC->PWMC_ISR & AT91C_PWMC_CHID2) == AT91C_PWMC_CHID2) {
    
    count++;
    
    // Fade in/out
    if (count == (PWM_FREQUENCY / (MAX_DUTY_CYCLE - MIN_DUTY_CYCLE))) {
      
      // Fade in
      if (fadeIn) {
	
	duty++;
	if (duty == MAX_DUTY_CYCLE) {
	  
	  fadeIn = 0;
	}
      }
      // Fade out
      else {
	
	duty--;
	if (duty == MIN_DUTY_CYCLE) {
	  
	  fadeIn = 1;
	}
      }
      
      // Set new duty cycle
      count = 0;
      PWMC_SetDutyCycle(CHANNEL_PWM_1, duty);
      PWMC_SetDutyCycle(CHANNEL_PWM_2, duty);
      PWMC_SetDutyCycle(CHANNEL_PWM_3, duty);
      PWMC_SetDutyCycle(CHANNEL_PWM_4, duty);
      //            PWMC_SetDutyCycle(CHANNEL_PWM, duty);
      //    }
  }
}

