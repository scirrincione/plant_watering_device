/*
 * File:   main.c
 * Author: icesk
 *
 * Created on November 14, 2024, 11:37 AM
 */


#include "main.h"

void initializeClock(){
    uint8_t temp;
    //RTC_CLKSEL_INT32K_gc
    /* Initialize 32.768kHz Oscillator: */
    /* Disable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_ENABLE_bm;
    /* Writing to protected register */
    ccp_write_io((void*) &CLKCTRL.XOSC32KCTRLA, temp);

    while (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm) {
        ; /* Wait until XOSC32KS becomes 0 */
    }

    /* SEL = 0 (Use External Crystal): */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_SEL_bm;
    /* Writing to protected register */
    ccp_write_io((void*) &CLKCTRL.XOSC32KCTRLA, temp);

    /* Enable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp |= CLKCTRL_ENABLE_bm;
    /* Writing to protected register */
    ccp_write_io((void*) &CLKCTRL.XOSC32KCTRLA, temp);

    /* Initialize RTC: */
    while (RTC.STATUS > 0) {
        ; /* Wait for all register to be synchronized */
    }
    /* Set period */
    RTC.PER = 1022;
    /* 32.768kHz External Crystal Oscillator (XOSC32K) */
    RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
    /* Run in debug: enabled */
    RTC.DBGCTRL |= RTC_DBGRUN_bm;

    RTC.CTRLA = RTC_PRESCALER_DIV32_gc /* 32 */
            | RTC_RTCEN_bm /* Enable: enabled */
            | RTC_RUNSTDBY_bm; /* Run In Standby: enabled */

    /* Enable Overflow Interrupt */
    RTC.INTCTRL |= RTC_OVF_bm;
}

void initializeAll(){
    initializeI2C();
    initializeTime();
    initializeClock();
    initializeButtons();
    currState = BASE_SCREEN;
    waterAmount = 5;
}

void baseScreen(){
    writeTime(clockTime.hours, clockTime.minutes);
    // use currMoisture, curr humidity, clockTime, and timeSinceWater
    // and send to display screen
}

void displayMoisture(){
    // display current moisture settings
    
    displayTitleNumber("", moistureThreshold);
}

void editMoisture(){
    // send edit screen to display
    // add inc to moisture var and display
    moistureThreshold+=inc;
    inc = 0;
    displayTitleNumber("", moistureThreshold);
}

void displayWaterAmount(){
    // display current water settings
    displayTitleNumber("", waterAmount);
}

void editWaterAmount(){
    // add inc and display
    waterAmount += inc;
    inc = 0;
    displayTitleNumber("", waterAmount);
}

void displayTimesWatered(){
    // display timeSinceWater
    displayTitleNumber("", timesWatered);
}

void displayAvgHumidity(){
    // display average humidity
    initializeADCSoil();
    int soilValue = getMoisture();
    ADC0.CTRLA = ADC_ENABLE_bm;
    displayTitleNumber("", soilValue);
}

void displayAvgWater(){
    // display amount water used
    int water = timesWatered*waterAmount;
    displayTitleNumber("", water);
}

void displayAvgTemp(){
    // display average temperature
    int avg = (int)tempAvg/tempAvgCount;
    displayTitleNumber("", avg);
}

void waterPlant(int moisture){
    // use motor and add certain amount of water to the plant
    //MOVE TO MOTOR HEADER FILE?
    
    //increment times of averages to record info
    sendCommandDisplay(CLEAR_SCREEN);
    waterUsed+=waterAmount;//replace 5 with amount watered each time
    waterAmountUsed = waterAmount;
    timesWatered++;
    prevState = currState;
    currState = WATERING;
    PORTA.OUTSET = PIN6_bm;
    currentlyWatering();
}

void recordAvgs(){
    //get all of the current values and add to avgs variables
    initializeADCSoil();
    int soilValue = getMoisture();
    ADC0.CTRLA = ADC_ENABLE_bm;
    
    initializeADCTemp();
    int tempVal = getTemp();
    tempAvg += tempVal;
    tempAvgCount++;
    ADC0.CTRLA = ADC_ENABLE_bm;
    
    if(soilValue < moistureThreshold){
        waterPlant(soilValue);
    }
}

void clockTick(){
    // increment clock time 
    clockTime = incrementTime(clockTime);
}

void initializeTime(){
    clockTime.hours = 12;
    clockTime.minutes = 0;
    clockTime.seconds = 0;
}

time_t incrementTime(time_t time){
    time.seconds = time.seconds + 1;
    if(time.seconds >= 60){
        time.seconds = 0;
        time.minutes++;
        if(time.minutes >= 60){
            time.hours++;
            time.minutes = 0;
            if(time.hours > 12)
                time.hours = 1;
    }
    }
    return time;
}

void currentlyWatering(){
    char message[4];
    sprintf(message, "%04d", waterAmountUsed);
    printMessage(message, 4);
    waterAmountUsed--;
    if(waterAmountUsed<=0){
            waterAmountUsed = waterAmount;
            currState = prevState;
            PORTA.OUTCLR = PIN6_bm;
        }
}

int main(void) {
    //sendCommandMonitor(0x80);
    //sendCommandMonitor(0x5D);
    
    //testSoilMonitor();
    initializeAll();
    sendCommandDisplay(CLEAR_SCREEN);
    
    //_delay_ms(10);
    sei();
    PORTA.DIRSET = PIN5_bm;
    PORTA.DIRSET = PIN6_bm;
    //PORTA.OUTSET = PIN6_bm;
    set_sleep_mode(SLEEP_MODE_STANDBY);
    while (1) {
        sleep_mode();
        
        if(clockTime.seconds == 10)
            recordAvgs();
        switch(currState){
            case(BASE_SCREEN):
                baseScreen();
                toggleColon(1);
                break;
            case(DISPLAY_MOISTURE):
                displayMoisture();
                toggleColon(0);
                break;
            case(EDIT_MOISTURE):
                editMoisture();
                toggleColon(0);
                break;
            case(DISPLAY_WATER_AMOUNT):
                displayWaterAmount();
                toggleColon(0);
                break;
            case(EDIT_WATER_AMOUNT):
                editWaterAmount();
                toggleColon(0);
                break;
            case(DISPLAY_AVG_TIME):
                displayTimesWatered();
                toggleColon(0);
                break;
            case(DISPLAY_AVG_HUMIDITY):
                displayAvgHumidity();
                toggleColon(0);
                break;
            case(DISPLAY_AVG_WATER):
                displayAvgWater();
                toggleColon(0);
                break;
            case(DISPLAY_AVG_TEMP):
                displayAvgTemp();
                toggleColon(0);
                break;
            case(WATERING):
                toggleColon(0);
                break;
        }
    }
    
    
}



ISR(PORTA_PORT_vect){
    /* Interrupt for the forward/settings button, on the base screen this button
       either traverses the settings for display moisture and water amount or 
       can be used to move back towards the base screen when used when on the 
       display averages screen*/
    if(PORTA.INTFLAGS & PIN7_bm){ //set button (forward/set)
        PORTA.OUTTGL = PIN5_bm;
        switch(currState){//if on base screen move to set screens
            case(BASE_SCREEN):
                currState = DISPLAY_MOISTURE;
                
                break;
            case(DISPLAY_MOISTURE):
                
                currState = DISPLAY_WATER_AMOUNT;
                break;
            case(EDIT_MOISTURE):
                inc++; //send signal to increase moisture by 1
                break;
            case(DISPLAY_WATER_AMOUNT):
                break;
            case(EDIT_WATER_AMOUNT):
                inc++;
                break;
            case(DISPLAY_AVG_TIME):
                
                currState = BASE_SCREEN;
                break;
            case(DISPLAY_AVG_HUMIDITY):
                currState = DISPLAY_AVG_TIME;
                break;
            case(DISPLAY_AVG_WATER):
                currState = DISPLAY_AVG_HUMIDITY;
                break;
            case(DISPLAY_AVG_TEMP):
                currState = DISPLAY_AVG_WATER;
                break;
            case(WATERING):
                break;
        }
        PORTA.INTFLAGS &= PIN7_bm;
    }
}

ISR(PORTC_PORT_vect){
    /* Interrupt for the confirm button, when on the settings screen this activates
       editing so the forward/settings button can be used to increase the number and the 
       backward/info button to decrease the number. When used on the info screens
       it brings the user back to the base screen (time)*/
    if(PORTC.INTFLAGS & PIN0_bm){ //set button (confirm) confirm engages edit 
                                  //mode or exits average screens
        PORTA.OUTTGL = PIN5_bm;
        switch(currState){
            case(BASE_SCREEN):
                //clock set?
                break;
            case(DISPLAY_MOISTURE):
                currState = EDIT_MOISTURE;
                break;
            case(EDIT_MOISTURE):
                currState = DISPLAY_MOISTURE;
                break;
            case(DISPLAY_WATER_AMOUNT):
                currState = EDIT_WATER_AMOUNT;
                break;
            case(EDIT_WATER_AMOUNT):
                currState = DISPLAY_WATER_AMOUNT;
                break;
            case(DISPLAY_AVG_TIME):
                currState = BASE_SCREEN;
                break;
            case(DISPLAY_AVG_HUMIDITY):
                currState = BASE_SCREEN;
                break;
            case(DISPLAY_AVG_WATER):
                currState = BASE_SCREEN;
                break;
            case(DISPLAY_AVG_TEMP):
                currState = BASE_SCREEN;
                break;
            case(WATERING):
                break;
        }
        PORTC.INTFLAGS &= PIN0_bm;
    }
    /* Backward/info button if on the base screen traverses the info screens or if
       on the averages screen it traverses towards the base screen*/
    if(PORTC.INTFLAGS & PIN1_bm){ 
        PORTA.OUTTGL = PIN5_bm;
        switch(currState){
            case(BASE_SCREEN):
                currState = DISPLAY_AVG_TIME;
                break;
            case(DISPLAY_MOISTURE):
                currState = BASE_SCREEN;
                break;
            case(EDIT_MOISTURE):
                inc--;
                break;
            case(DISPLAY_WATER_AMOUNT):
                currState = DISPLAY_MOISTURE;
                break;
            case(EDIT_WATER_AMOUNT):
                inc--;
                break;
            case(DISPLAY_AVG_TIME):
                currState = DISPLAY_AVG_HUMIDITY;
                break;
            case(DISPLAY_AVG_HUMIDITY):
                currState = DISPLAY_AVG_WATER;
                break;
            case(DISPLAY_AVG_WATER):
                currState = DISPLAY_AVG_TEMP;
                break;
            case(DISPLAY_AVG_TEMP):
                break;
            case(WATERING):
                break;
        }
        PORTC.INTFLAGS &= PIN1_bm;
    }
}

/* Interrupt for the clock, goes off every second and increments the time and 
   if currently watering the plan calls function for decrementing the timer by 1*/
ISR(RTC_CNT_vect) {
    clockTick();
    if(currState == WATERING)
        currentlyWatering();
    RTC.INTFLAGS = RTC_OVF_bm;
}