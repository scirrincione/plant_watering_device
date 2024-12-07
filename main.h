/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MAIN_H_
#define	MAIN_H_
#define F_CPU 3333333

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>



#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>

#include "ADC.h"

#include "buttonsHeader.h"

#include "I2C.h"

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations

// Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation
// state for all of the possible screens
enum State{
    BASE_SCREEN,
    DISPLAY_MOISTURE,
    EDIT_MOISTURE,
    DISPLAY_WATER_AMOUNT,
    EDIT_WATER_AMOUNT,
    DISPLAY_AVG_TIME,
    DISPLAY_AVG_HUMIDITY,
    DISPLAY_AVG_WATER,
    DISPLAY_AVG_TEMP,
    WATERING
};
typedef struct {
    int hours;
    int minutes;
    int seconds;
} time_t ;

// Variable for current state of device
volatile enum State currState;

// Variable to keep track of how much water has been pumped to the plant by the 
// device
volatile int waterUsed = 0;

// Variable to keep track of average temperature near plant
volatile float tempAvg = 0;
volatile int tempAvgCount = 0;

// Variable to keep track of the number of times the plant has been watered
volatile int timesWatered = 0;

// Variables for watering including what level to water at and how much water to
// use as well as variable to keep track of the number of times the plant's been
// watered
volatile int moistureThreshold = 50;
volatile int waterAmount;
volatile int waterAmountUsed;

// prevState to keep track of what was previously on display before watering 
// happened
volatile enum State prevState;

// inc to keep track of what to increment or decrement moistureThreshold or waterAmount
volatile int inc;
//volatile int moisture = -1;

// variable to keep track of time
time_t clockTime;

// Initialize rtc counter
void initializeClock();

// Method to call that initializes everything that needs to be initialized on startup
void initializeAll();

// Sends commands for displaying base screen
void baseScreen();

// Sends commands for displaying moisture
void displayMoisture();

// // Sends commands for editing moisture
void editMoisture();

// Sends commands for displaying water amount used on the plant during interrupts
void displayWaterAmount();

// Sends commands for editing water amount to use
void editWaterAmount();

// Changed this to instead show number of times watered
void displayAvgTimeBetweenWatering();

// Changed this to show the current soil monitor value
void displayAvgHumidity();

// Show how much water used overall
void displayAvgWater();

// Show average temp near plant
void displayAvgTemp();

// Called when water plant first goes off 
void waterPlant(int moisture);

// Called when it's time to check if plant needs to be watered and also record
// temp average
void recordAvgs();

// To be called every time rtc interrupt goes off 
void clockTick();

// initializes time variables
void initializeTime();

// Used to increment time variable by 1 second
time_t incrementTime(time_t time);

// Called while watering is currently going on after waterPlant is called,
// turns off pump when enough water has been used
void currentlyWatering();

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

