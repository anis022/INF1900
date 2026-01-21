#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

enum class State
{
    REDINIT,
    AMBERHOLD,
    GREENRELEASE,
    REDHOLD,
    OFFRELEASE,
    GREENHOLD
};

const uint8_t debounceDelay = 30;
const uint8_t redDelay = 3;
const uint8_t greenDelay = 1;

bool buttonIsPressed()
{
    if (PIND & (1 << PD2))
    {
        _delay_ms(debounceDelay);
        if (PIND & (1 << PD2))
        {
            return true;
        }
    }
    return false;
}

void greenLight()
{
    PORTA |= (1 << PA0);
    PORTA &= ~(1 << PA1);
}
void redLight()
{
    PORTA |= (1 << PA1);
    PORTA &= ~(1 << PA0);
}

void amberLight()
{
    redLight();
    _delay_ms(redDelay);
    greenLight();
    _delay_ms(greenDelay);
}

void lightOff()
{
    PORTA &= ~(1 << PA1);
    PORTA &= ~(1 << PA0);
}

int main()
{

    State state = State::REDINIT;

    DDRA |= (1 << PA0) | (1 << PA1);
    DDRD &= ~(1 << PD2); // XXXX XX0X

    while (true)
    {
        switch (state)
        {
        case State::REDINIT:
            if (buttonIsPressed())
            {
                state = State::AMBERHOLD;
            }
            break;

        case State::AMBERHOLD:
            if (!buttonIsPressed())
            {
                state = State::GREENRELEASE;
            }
            break;

        case State::GREENRELEASE:
            if (buttonIsPressed())
            {
                state = State::REDHOLD;
            }
            break;

        case State::REDHOLD:
            if (!buttonIsPressed())
            {
                state = State::OFFRELEASE;
            }
            break;

        case State::OFFRELEASE:
            if (buttonIsPressed())
            {
                state = State::GREENHOLD;
            }
            break;

        case State::GREENHOLD:
            if (!buttonIsPressed())
            {
                state = State::REDINIT;
            }
            break;
        }

        switch (state)
        {
        case State::REDINIT:
            redLight();
            break;

        case State::AMBERHOLD:
            while (buttonIsPressed())
            {
                amberLight();
            }
            break;

        case State::GREENRELEASE:
            greenLight();
            break;

        case State::REDHOLD:
            redLight();
            break;

        case State::OFFRELEASE:
            lightOff();
            break;

        case State::GREENHOLD:
            greenLight();
            break;
        }
    }
    return 0;
}