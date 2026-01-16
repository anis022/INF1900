#include <avr/io.h> 
#include <util/delay.h>

int main()
{

    DDRA |= (1 << PA0)|(1 << PA1);
    DDRD &= ~(1 << PD2); // xxxx xx0x


    while(true)
    {

        if (PIND & (1 << PD2)) // XXXX XXXX & 0000 0100 = 0000 0X00
        {
            _delay_ms(30);
            if (PIND & (1 << PD2))
            {
                PORTA |= (1 << PA1);
                PORTA &=~(1 << PA0);
            }
            
        }
        else
        {
            PORTA |= (1 << PA0);
            PORTA &= ~(1 << PA1);

        }


    }   

    return 0;
}


