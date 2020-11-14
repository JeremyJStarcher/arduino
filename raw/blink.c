#define F_CPU 16000000UL
#define LED_PIN 5

#include <avr/io.h>
#include <util/delay.h>

const int delay_ms = 600;

int main(void) 
{
  DDRB |= (1 << LED_PIN);

  while (1) 
  {
    PORTB ^= (1 << LED_PIN);
    _delay_ms(delay_ms);
    PORTB ^= (1 << LED_PIN);
    _delay_ms(delay_ms);
  }
  return 0;
}
