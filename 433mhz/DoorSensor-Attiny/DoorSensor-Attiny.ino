#include <avr/sleep.h>
#include <avr/power.h>

#include <NewRemoteTransmitter.h>

// Utility macros
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC

int pinReed = 0;

unsigned long address = 28364062;
const byte unit = 9;
const byte pinTransmitter = 1;
unsigned int periodusec = 260;
const byte repeats = 3;


//interrupt
ISR (PCINT0_vect) 
{
  if(digitalRead(0) == HIGH) {
    transmit(unit, true);    
  }
}

void setup() {
  pinMode(pinReed,INPUT_PULLUP);
  pinMode(pinTransmitter,OUTPUT);

  // pin change interrupt
  PCMSK  |= bit (PCINT0);
  GIFR   |= bit (PCIF);    // clear any outstanding interrupts
  GIMSK  |= bit (PCIE);    // enable pin change interrupts
}

void loop() {
  delay(300000);
  transmit(unit, false);
  goToSleep();
}


//transmit function
void transmit(byte unit, boolean switchOn) {
  NewRemoteTransmitter transmitter(address, pinTransmitter, periodusec, repeats);
  transmitter.sendUnit(unit, switchOn);
}

void goToSleep () {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();                             
  sleep_disable();   
  power_all_enable();    // power everything back on
}
