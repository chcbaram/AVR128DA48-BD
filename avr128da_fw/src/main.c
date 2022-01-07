#include <avr/io.h>



int main(void) 
{
  PORTC.DIRSET = (1<<6);

  while(1)
  {
    PORTC.OUTTGL = (1<<6);

    for (volatile uint32_t i=0; i<10000*2; i++)
    {
    }    
  }

  return 0;
}
