/*

Auteurs : Jérémie Anglaret-Guirguis, Anis Benabdallah
Travail : TP2
Section # : 05
Équipe # : 111
Correcteur : Meriam Ben Rabia

Description du programme : Ce programme implémente une machine à états pour contrôler une LED bicolore (rouge et verte) en fonction de l'état d'un bouton poussoir. La LED passe par plusieurs états (INIT, FIRST_PRESS, SECOND_PRESS, THIRD_PRESS et WAIT_FOR_RELEASE) en fonction des pressions et relâchements du bouton. À l'état initial, la LED est éteinte et reste éteinte jusqu'à la troixième pression du bouton, où elle s'allume en vert pour deux secondes avant de revenir à l'état initial.

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
* LED+ : Green led
* LED- : Red led
* =====================================================================


+----------------+--------+---------------+--------------+-----------+
|                |  Input |               |              |   Output  |
| Current State  +--------+ Inter. State  |  Next State  +-----+-----+
|                | Button |               |              | PA0 | PA1 |
+----------------+--------+---------------+--------------+-----+-----+
|      INIT      |    0   |      INIT     |     INIT     |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|      INIT      |    1   | W._F._RELEASE |  FIRST_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|   FIRST_PRESS  |    0   |  FIRST_PRESS  |  FIRST_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|   FIRST_PRESS  |    1   | W._F._RELEASE | SECOND_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|  SECOND_PRESS  |    0   |  SECOND_PRESS | SECOND_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|  SECOND_PRESS  |    1   | W._F._RELEASE |  THIRD_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|   THIRD_PRESS  |    0   |               |     INIT     |  1  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|   THIRD_PRESS  |    1   |               |     INIT     |  1  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|                |        |  FIRST_PRESS  |  FIRST_PRESS |  0  |  0  |
|                |        +---------------+--------------+-----+-----+
|  W._F._RELEASE |    0   |  SECOND_PRESS | SECOND_PRESS |  0  |  0  |
|                |        +---------------+--------------+-----+-----+
|                |        |  THIRD_PRESS  |  THIRD_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
|                |        |               |  FIRST_PRESS |  0  |  0  |
|                |        +---------------+--------------+-----+-----+
|  W._F._RELEASE |    1   |               | SECOND_PRESS |  0  |  0  |
|                |        +---------------+--------------+-----+-----+
|                |        |               |  THIRD_PRESS |  0  |  0  |
+----------------+--------+---------------+--------------+-----+-----+
LEGENDE :
- Inter. State = Intermediate State
- W._F._RELEASE = WAIT_FOR_RELEASE
- PA0 = 0: LED OFF
- PA0 = 1: LED GREEN
*/


#define F_CPU 8000000UL
#include <avr/io.h> 
#include <util/delay.h>

const uint16_t GREEN_DELAY = 2000;
const uint8_t DEBOUNCE_DELAY = 30;

enum class State
{
    INIT,
    FIRST_PRESS,
    SECOND_PRESS,
    THIRD_PRESS,
    WAIT_FOR_RELEASE
};

bool isPressed()
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

void turnOffLED()
{
    PORTA &=~(1 << PA0);
    PORTA &=~(1 << PA1);
}

void turnOnLED()
{
    PORTA |= (1 << PA0);
    PORTA &=~(1 << PA1);
}

void switchLogic(State& currentState, State& pendingState)
{
    switch(currentState)
    {
        case State::INIT:
            if (isPressed())
            {
                pendingState = State::FIRST_PRESS;
                currentState = State::WAIT_FOR_RELEASE;
            }
            break;

        case State::FIRST_PRESS:
            if (isPressed())
            {
                pendingState = State::SECOND_PRESS;
                currentState = State::WAIT_FOR_RELEASE;
            }
            break;

        case State::SECOND_PRESS:
            if (isPressed())
            {
                pendingState = State::THIRD_PRESS;
                currentState = State::WAIT_FOR_RELEASE;
            }
            break;

        case State::THIRD_PRESS:
            _delay_ms(GREEN_DELAY);
            currentState = State::INIT;
            break;

        case State::WAIT_FOR_RELEASE:
            if (!isPressed())
            {
                currentState = pendingState;
            }
            break;
    }

}

void lightLogic(State& currentState, State& pendingState)
{
    switch(currentState)
    {
        case State::INIT:
            turnOffLED();
            break;

        case State::FIRST_PRESS:
            turnOffLED();
            break;

        case State::SECOND_PRESS:
            turnOffLED();
            break;

        case State::THIRD_PRESS:
            turnOnLED();
            _delay_ms(GREEN_DELAY);
            currentState = State::INIT;
            break;

        case State::WAIT_FOR_RELEASE:
            turnOffLED();
            break;
    }
}

int main()
{
    State currentState = State::INIT;
    State pendingState = State::INIT;
    DDRA |= (1 << PA0)|(1 << PA1);
    DDRD &= ~(1 << PD2); // XXXX XX0X

    while (true)
    {
        switchLogic(currentState, pendingState);
        lightLogic(currentState, pendingState);
    }
    return 0;
}

