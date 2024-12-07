/*
 * File:   ADC.c
 * Author: icesk
 *
 * Created on November 23, 2024, 3:37 PM
 */


#include <avr/io.h>
#include "ADC.h"

void initializeADCSoil(){
    PORTD.DIRCLR = PIN6_bm;
    PORTD.PIN6CTRL &= ~PORT_ISC_gm;
    PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;
    ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc;
    ADC0.CTRLC = ADC_PRESC_DIV4_gc | ADC_REFSEL_VDDREF_gc; /* Internal reference */
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc | ADC_RUNSTBY_bm;
    //ADC0.INTCTRL |= ADC_RESRDY_bm;
    //ADC0.COMMAND = ADC_STCONV_bm;
    
}

void initializeADCTemp(){
    PORTD.DIRCLR = PIN1_bm;
    PORTD.PIN1CTRL &= ~PORT_ISC_gm;
    PORTD.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTD.PIN1CTRL &= ~PORT_PULLUPEN_bm;
    ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc;
    ADC0.CTRLC = ADC_PRESC_DIV4_gc | ADC_REFSEL_VDDREF_gc; /* Internal reference */
    ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc | ADC_RUNSTBY_bm;
    //ADC0.INTCTRL |= ADC_RESRDY_bm;
    //ADC0.COMMAND = ADC_STCONV_bm;
    
}

uint8_t getMoisture(){
    ADC0.COMMAND = ADC_STCONV_bm;
    while(!(ADC0.INTFLAGS & ADC_RESRDY_bm)){;}
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    return ADC0.RES;
}

uint8_t getTemp(){
    ADC0.COMMAND = ADC_STCONV_bm;
    while(!(ADC0.INTFLAGS & ADC_RESRDY_bm)){;}
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    return ((ADC0.RES)/10)*9/5+32;
}