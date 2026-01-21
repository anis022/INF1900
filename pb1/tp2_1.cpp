#define F_CPU 8000000UL
#include <avr/io.h> 
#include <util/delay.h>



enum class State
{
    INIT,
    FIRST_PRESS,
    SECOND_PRESS,
    THIRD_PRESS,
    BUTTON_DOWN
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
// Turn LED OFF
void lightOff()
{
    PORTA &=~(1 << PA0);
    PORTA &=~(1 << PA1);
}

// Turn LED on
void lightOn()
{
    PORTA |= (1 << PA1);
    PORTA &=~(1 << PA0);
}

int main()
{
    State state = State::INIT;
    State nextState = State::INIT;
    DDRA |= (1 << PA0)|(1 << PA1);
    DDRD &= ~(1 << PD2); // XXXX XX0X

    while (true)
    {
        switch(state)
        {
            case State::INIT:
                if (isPressed())
                {
                    nextState = State::FIRST_PRESS;
                    state = State::BUTTON_DOWN;
                }
                break;

            case State::FIRST_PRESS:
                if (isPressed())
                {
                    nextState = State::SECOND_PRESS;
                    state = State::BUTTON_DOWN;
                }
                break;

            case State::SECOND_PRESS:
                if (isPressed())
                {
                    nextState = State::THIRD_PRESS;
                    state = State::BUTTON_DOWN;
                }
                break;

            case State::THIRD_PRESS:
                _delay_ms(2000);
                state = State::INIT;
                break;

            case State::BUTTON_DOWN:
                if (!isPressed())
                {
                    state = nextState;
                }
                break;
        }
        
        switch(state)
        {
            case State::INIT:
                lightOff();
                break;

            case State::FIRST_PRESS:
                lightOff();
                break;

            case State::SECOND_PRESS:
                lightOff();
                break;

            case State::THIRD_PRESS:
                lightOn();
                _delay_ms(2000);
                state = State::INIT;
                break;

            case State::BUTTON_DOWN:
                lightOff();
                break;
        }
    }
}