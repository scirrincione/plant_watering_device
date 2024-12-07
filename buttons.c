/*
 * File:   buttons.c
 * Author: icesk
 *
 * Created on November 14, 2024, 11:16 AM
 */


#include <avr/io.h>
#include "buttonsHeader.h"

void initializeButtons(){
    /*for(int i = 0; i < buttonsLength; i++){
        buttons[i].port->DIRCLR = buttons[i].pin;
        (&(buttons[i].port->PIN0CTRL))[buttons[i].pin] = (PORT_PULLUPEN_bm | PORT_ISC_RISING_gc);
    }*/
    PORTA.DIRCLR = PIN7_bm;
    PORTC.DIRCLR = PIN0_bm;
    PORTC.DIRCLR = PIN1_bm;
    PORTA.PIN7CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_RISING_gc);
    PORTC.PIN0CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_RISING_gc);
    PORTC.PIN1CTRL |= (PORT_PULLUPEN_bm | PORT_ISC_RISING_gc);
}
