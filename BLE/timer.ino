#include <sam.h>
#include "timer.h"

/*
   Stop the timer, clear out any timer state and reset all counters. Set the
   clock the timer uses and enable interrupts.
*/
void timer3_init() {

  // disable timer setting to so we can modify settings
  TC3->COUNT8.CTRLA.bit.ENABLE = 0;

  //must wait for ENABLE bit to be turned off
  while (TC3->COUNT8.CTRLA.bit.ENABLE);

  //setting the prescaler and counter synchronization bit group
  TC3->COUNT8.CTRLA.bit.PRESCSYNC = 0x3;

  // selecting clock and related settings, must connect timer to clock
  
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) | GCLK_GENDIV_ID(1) ;
  GCLK->GENCTRL.reg = GCLK_GENCTRL_SRC_OSC32K | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_ID(1);
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN_GCLK1 | GCLK_CLKCTRL_ID_TCC2_TC3 | GCLK_CLKCTRL_CLKEN;

  TC3->COUNT8.CTRLA.bit.PRESCALER = 0x7;  // slowing down by factor of 1024
  TC3->COUNT8.CTRLA.bit.MODE = 0x1; // only need 8 bits to count

  // clear out timer state
  TC3->COUNT8.COUNT.bit.COUNT = 0;
  // enable interrupts
  TC3->COUNT8.INTENSET.bit.MC0 = 1;

  timer3_set(3);

  //redirect handler req to proc
  NVIC_SetPriority(TC3_IRQn, 0); //hihest pri
  NVIC_EnableIRQ(TC3_IRQn); //enabling interr
  __enable_irq(); //global func


  //even 1s - parity 0
}

/*
   Reset timer, but not the peripheral. Timer can be in middle of execution
   when reset.
*/
void timer3_reset() {

  TC3->COUNT8.CTRLA.bit.ENABLE = 0;
  while (TC3->COUNT8.CTRLA.bit.ENABLE);

  TC3->COUNT8.COUNT.bit.COUNT = 0;
  // clear the interrupt flag
  TC3->COUNT8.INTFLAG.bit.MC0 = 1;
  // reenable
  TC3->COUNT8.CTRLA.bit.ENABLE = 1;
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);
}

/*
   Set period that the timer will fire. Timer interrupt should be fired for
   each timer period.
*/
void timer3_set( uint16_t period_ms ) {
  /*
     Set based on calculations assuming 32 kHz clock that has been prescaled
     by 16
  */

  TC3->COUNT8.CTRLA.bit.ENABLE = 0;
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY); //changed to sncbusy

  TC3->COUNT8.CTRLA.bit.WAVEGEN = 0x1;  /* because MFRQ lets us set an upper
                                         bound on when we want the timer to
                                         stop
*/

  TC3->COUNT8.CC[0].reg = (uint8_t)period_ms; /* upper bound for timer counting (fires an interrupt)
                                        tell which clock to use (in our case, the 32kHz
                                        with 32 pre scaler)
                            */

  TC3->COUNT8.CTRLA.bit.ENABLE = 1;
  while (TC3->COUNT8.STATUS.bit.SYNCBUSY);


}
