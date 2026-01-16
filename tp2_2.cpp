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

bool isPressed()
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

void green()
{
    PORTA |= (1 << PA0);
    PORTA &= ~(1 << PA1);
}
void red()
{
    PORTA |= (1 << PA1);
    PORTA &= ~(1 << PA0);
}

void amber()
{
    red();
    _delay_ms(3);
    green();
    _delay_ms(1);
}

void off()
{
    PORTA &= ~(1 << PA1);
    PORTA &= ~(1 << PA0);
}

int main()
{

    State state = State::REDINIT;

    DDRA |= (1 << PA0) | (1 << PA1);
    DDRD &= ~(1 << PD2); // xxxx xx0x

    while (true)
    {
        switch (state)
        {
        case State::REDINIT:
            if (isPressed())
            {
                state = State::AMBERHOLD;
            }
            break;

        case State::AMBERHOLD:
            if (!isPressed())
            {
                state = State::GREENRELEASE;
            }
            break;

        case State::GREENRELEASE:
            if (isPressed())
            {
                state = State::REDHOLD;
            }
            break;

        case State::REDHOLD:
            if (!isPressed())
            {
                state = State::OFFRELEASE;
            }
            break;

        case State::OFFRELEASE:
            if (isPressed())
            {
                state = State::GREENHOLD;
            }
            break;

        case State::GREENHOLD:
            if (!isPressed())
            {
                state = State::REDINIT;
            }
            break;
        }

        switch (state)
        {
        case State::REDINIT:
            red();
            break;

        case State::AMBERHOLD:
            while (isPressed())
            {
                amber();
                // if (!ispressed()){
                //     break;
            }
            break;

        case State::GREENRELEASE:
            green();
            break;

        case State::REDHOLD:
            red();
            break;

        case State::OFFRELEASE:
            off();
            break;

        case State::GREENHOLD:
            green();
            break;
        }
    }
    return 0;
}