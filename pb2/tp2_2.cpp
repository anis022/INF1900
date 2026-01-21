#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

enum class State
{
    RED_INIT,
    AMBER_HOLD,
    GREEN_RELEASE,
    RED_HOLD,
    OFF_RELEASE,
    GREEN_HOLD
};

const uint8_t DEBOUNCE_DELAY = 30;
const uint8_t RED_DELAY = 4;
const uint8_t GREEN_DELAY = 1;

bool buttonIsPressed()
{
    if (PIND & (1 << PD2))
    {
        _delay_ms(DEBOUNCE_DELAY);
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
    _delay_ms(RED_DELAY);
    greenLight();
    _delay_ms(GREEN_DELAY);
}

void lightOff()
{
    PORTA &= ~(1 << PA1);
    PORTA &= ~(1 << PA0);
}

void switchLogic(State& state)
{
    switch (state)
    {
    case State::RED_INIT:
        if (buttonIsPressed())
        {
            state = State::AMBER_HOLD;
        }
        break;

    case State::AMBER_HOLD:
        if (!buttonIsPressed())
        {
            state = State::GREEN_RELEASE;
        }
        break;

    case State::GREEN_RELEASE:
        if (buttonIsPressed())
        {
            state = State::RED_HOLD;
        }
        break;

    case State::RED_HOLD:
        if (!buttonIsPressed())
        {
            state = State::OFF_RELEASE;
        }
        break;

    case State::OFF_RELEASE:
        if (buttonIsPressed())
        {
            state = State::GREEN_HOLD;
        }
        break;

    case State::GREEN_HOLD:
        if (!buttonIsPressed())
        {
            state = State::RED_INIT;
        }
        break;
    }
}

void lightLogic(State& state)
{
    switch (state)
    {
    case State::RED_INIT:
        redLight();
        break;

    case State::AMBER_HOLD:
            amberLight();
        break;

    case State::GREEN_RELEASE:
        greenLight();
        break;

    case State::RED_HOLD:
        redLight();
        break;

    case State::OFF_RELEASE:
        lightOff();
        break;

    case State::GREEN_HOLD:
        greenLight();
        break;
    }
}


int main()
{

    State state = State::RED_INIT;

    DDRA |= (1 << PA0) | (1 << PA1);
    DDRD &= ~(1 << PD2); // XXXX XX0X

    while (true)
    {
        switchLogic(state);
        lightLogic(state);

    }
    return 0;
}