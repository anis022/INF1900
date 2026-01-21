/*


Travail : TP2
Section # : 05
Équipe # : 111
Correcteur : Meriam Ben Rabia

* ========== I/O IDENTIFICATION (CONNEXIONS SUR LE ROBOT) =============
* 
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
* LED+ : green led
* LED- : red led
* =====================================================================

+----------------+--------+---------------+--------------+-----------+
|                |  Input |               |              |   Output  |
| Previous State +--------+ Current State |  Next State  +-----+-----+
|                | Button |               |              | PA0 | PA1 |
+----------------+--------+---------------+--------------+-----+-----+
|      INIT      |    0   |      INIT     |     INIT     |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|      INIT      |    1   |  BUTTON_DOWN  |  FIRST_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|   FIRST_PRESS  |    0   |  FIRST_PRESS  |  FIRST_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|   FIRST_PRESS  |    1   |  BUTTON_DOWN  | SECOND_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|  SECOND_PRESS  |    0   |  SECOND_PRESS | SECOND_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|  SECOND_PRESS  |    1   |  BUTTON_DOWN  |  THIRD_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|   THIRD_PRESS  |    0   |      INIT     |       X      |  1  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|   THIRD_PRESS  |    1   |      INIT     |       X      |  1  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|                |        |  FIRST_PRESS  |  FIRST_PRESS |  0  |  0  |
|                |        +---------------+--------------+-----+-----+
|   BUTTON_DOWN  |    0   |  SECOND_PRESS | SECOND_PRESS |  0  |  0  |
|                |        +---------------+--------------+-----+-----+
|                |        |  THIRD_PRESS  |  THIRD_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|                |        |  BUTTON_DOWN  |  FIRST_PRESS |  0  |  0  |
|                |        +---------------+--------------+-----+-----+
|   BUTTON_DOWN  |    1   |  BUTTON_DOWN  | SECOND_PRESS |  0  |  0  |
|                |        +---------------+--------------+-----+-----+
|                |        |  BUTTON_DOWN  |  THIRD_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+

*/






#define F_CPU 8000000UL
#include <avr/io.h> 
#include <util/delay.h>

const uint16_t greenDelay = 2000;

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
    PORTA |= (1 << PA0);
    PORTA &=~(1 << PA1);
}

void switchLogic(State& currentState, State& nextState)
{
    switch(currentState)
    {
        case State::INIT:
            if (isPressed())
            {
                nextState = State::FIRST_PRESS;
                currentState = State::BUTTON_DOWN;
            }
            break;

        case State::FIRST_PRESS:
            if (isPressed())
            {
                nextState = State::SECOND_PRESS;
                currentState = State::BUTTON_DOWN;
            }
            break;

        case State::SECOND_PRESS:
            if (isPressed())
            {
                nextState = State::THIRD_PRESS;
                currentState = State::BUTTON_DOWN;
            }
            break;

        case State::THIRD_PRESS:
            _delay_ms(2000);
            currentState = State::INIT;
            break;

        case State::BUTTON_DOWN:
            if (!isPressed())
            {
                currentState = nextState;
            }
            break;
    }

}

void lightLogic(State& currentState, State& nextState)
{
    switch(currentState)
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
            _delay_ms(greenDelay);
            currentState = State::INIT;
            break;

        case State::BUTTON_DOWN:
            lightOff();
            break;
    }
}

int main()
{
    State currentState = State::INIT;
    State nextState = State::INIT;
    DDRA |= (1 << PA0)|(1 << PA1);
    DDRD &= ~(1 << PD2); // XXXX XX0X

    while (true)
    {
        switchLogic(currentState, nextState);
        lightLogic(currentState, nextState);
    }
    return 0;
}