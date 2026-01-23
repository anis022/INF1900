
/*
Auteurs : Jérémie Anglaret-Guirguis, Anis Benabdallah
Travail : TP2
Section # : 05
Équipe # : 111
Correcteur : Meriam Ben Rabia

Description du programme : Ce programme implémente une machine à états pour contrôler une LED bicolore (rouge et verte) en fonction de l'état d'un bouton poussoir. La LED passe par plusieurs états (RED_INIT, AMBER_HOLD, GREEN_RELEASE, etc.) en fonction des pressions et relâchements du bouton.

* ========== I/O IDENTIFICATION (CONNEXIONS SUR LE ROBOT) =============
* 
* ┌───────────────────────────────────────────────────────────────────┐
* │                    ATmega324PA - Pinout                           │
* ├───────────────────────────────────────────────────────────────────┤
* │                                                                   │
* │   PORTA            PORTB           PORTC          PORTD           │
* │  ┌───────┐        ┌──────┐         ┌──────┐       ┌──────┐        │   
* │  │ PA0 ●-│→ LED+  │PB0 ○ │         │PC0 ○ │       │PD0 ○ │        │
* │  │ PA1 ●-│→ LED-  │PB1 ○ │         │PC1 ○ │       │PD1 ○ │        │
* │  │ PA2 ○ │        │PB2 ○ │         │PC2 ○ │       │PD2 ●-│→B-INT0 │
* │  │ PA3 ○ │        │PB3 ○ │         │PC3 ○ │       │PD3 ○ │        │
* │  │ PA4 ○ │        │PB4 ○ │         │PC4 ○ │       │PD4 ○ │        │
* │  │ PA5 ○ │        │PB5 ○ │         │PC5 ○ │       │PD5 ○ │        │
* │  │ PA6 ○ │        │PB6 ○ │         │PC6 ○ │       │PD6 ○ │        │
* │  │ PA7 ○ │        │PB7 ○ │         │PC7 ○ │       │PD7 ○ │        │
* │  └───────┘        └──────┘         └──────┘       └──────┘        │
* │                                                                   │
* │  ● = Utilisé          ○ = Non utilisé                             │
* └───────────────────────────────────────────────────────────────────┘
* LEGENDE :
* PG : SPI (Programmeur)
* RXD/TXD : UART (debug)
* SCL/SDA : EEPROM externe
* B-INT0 : bouton
* LED+ : Green led
* LED- : Red led
* =====================================================================

+---------------+--------+---------------+--------+
|               |  INPUT |               | OUTPUT |
| Current State +--------+   Next State  +--------+
|               | Button |               |   LED  |
+---------------+--------+---------------+--------+
|    RED_INIT   |    0   |    RED_INIT   |   Red  |
+---------------+--------+---------------+--------+
|    RED_INIT   |    1   |   AMBER_HOLD  |   Red  |
+---------------+--------+---------------+--------+
|   AMBER_HOLD  |    0   | GREEN_RELEASE |  Amber |
+---------------+--------+---------------+--------+
|   AMBER_HOLD  |    1   |   AMBER_HOLD  |  Amber |
+---------------+--------+---------------+--------+
| GREEN_RELEASE |    0   | GREEN_RELEASE |  Green |
+---------------+--------+---------------+--------+
| GREEN_RELEASE |    1   |    RED_HOLD   |  Green |
+---------------+--------+---------------+--------+
|    RED_HOLD   |    0   |  OFF_RELEASE  |   Red  |
+---------------+--------+---------------+--------+
|    RED_HOLD   |    1   |    RED_HOLD   |   Red  |
+---------------+--------+---------------+--------+
|  OFF_RELEASE  |    0   |  OFF_RELEASE  |   Off  |
+---------------+--------+---------------+--------+
|  OFF_RELEASE  |    1   |   GREEN_HOLD  |   Off  |
+---------------+--------+---------------+--------+
|   GREEN_HOLD  |    0   |    RED_INIT   |  Green |
+---------------+--------+---------------+--------+
|   GREEN_HOLD  |    1   |   GREEN_HOLD  |  Green |
+---------------+--------+---------------+--------+

*/




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