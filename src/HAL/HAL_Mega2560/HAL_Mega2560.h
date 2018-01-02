#include <EEPROM.h>

// Distance in arc-min ahead of and behind the current Equ position, used for rate calculation
#define RefractionRateRange 30

// Fast port writing help
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

extern long int siderealInterval;
extern void SetSiderealClockRate (long int);

void HAL_Init_Timer_Sidereal() {
  // initialize the timers that handle the sidereal clock, RA, and Dec
  SetSiderealClockRate(siderealInterval);
}

// Extra stuff to be done to timers
void HAL_Init_Timers_Extra() {
  // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR3B = (1 << WGM12) | (1 << CS11);
  TCCR3A = 0;
  TIMSK3 = (1 << OCIE3A);

  // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR4B = (1 << WGM12) | (1 << CS11);
  TCCR4A = 0;
  TIMSK4 = (1 << OCIE4A);
}
