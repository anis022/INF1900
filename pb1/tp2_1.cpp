#define F_CPU 8000000
#include <avr/io.h> 
#include <util/delay.h>



enum class State
{
    INIT,
    FIRSTPRESS,
    SECONDPRESS,
    THIRDPRESS,
    PRESSED
};

bool isPressed()
{
    if (PIND & (1 << PD2))
    {
        _delay_ms(30);
        if (PIND & (1 << PD2))
        {
            return true;
        }
    }
    return false;
}

int main()
{
    State state = State::INIT;
    State nextState = State::INIT;
    DDRA |= (1 << PA0)|(1 << PA1);
    DDRD &= ~(1 << PD2); // xxxx xx0x

    while (true)
    {
        switch(state)
        {
            case State::INIT:
                if (isPressed())
                {
                    nextState = State::FIRSTPRESS;
                    state = State::PRESSED;
                }
                break;

            case State::FIRSTPRESS:
                if (isPressed())
                {
                    nextState = State::SECONDPRESS;
                    state = State::PRESSED;
                }
                break;

            case State::SECONDPRESS:
                if (isPressed())
                {
                    nextState = State::THIRDPRESS;
                    state = State::PRESSED;
                }
                break;

            case State::THIRDPRESS:
                _delay_ms(2000);
                state = State::INIT;
                break;

            case State::PRESSED:
                if (!isPressed())
                {
                    state = nextState;
                }
                break;
        }
        
        switch(state)
        {
            case State::INIT:
                PORTA &=~(1 << PA0);
                PORTA &=~(1 << PA1);
                break;

            case State::FIRSTPRESS:
                PORTA &=~(1 << PA0);
                PORTA &=~(1 << PA1);
                break;

            case State::SECONDPRESS:
                PORTA &=~(1 << PA0);
                PORTA &=~(1 << PA1);
                break;

            case State::THIRDPRESS:
                PORTA |= (1 << PA1);
                PORTA &=~(1 << PA0);
                _delay_ms(2000);
                state = State::INIT;
                break;

            case State::PRESSED:
                PORTA &=~(1 << PA0);
                PORTA &=~(1 << PA1);
                break;
        }
    }
}