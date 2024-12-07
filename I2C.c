/*
 * File:   I2C.c
 * Author: icesk
 *
 * Created on November 12, 2024, 3:18 PM
 */


#include <avr/io.h>
#include "I2C.h"

void initializeI2C(){
    PORTB.DIRSET = PIN2_bm | PIN3_bm;
    PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
    PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
    
    TWI0.CTRLA = TWI_SDAHOLD_50NS_gc;
    
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
    TWI0.MBAUD = 10;
    TWI0.MCTRLA = TWI_ENABLE_bm;
}


uint8_t readMonitor(uint8_t* reading){
    uint8_t len = 6;
    
    TWI0.MCTRLB = TWI_MCMD_REPSTART_gc;
    
    TWI0.MADDR = (HUMIDITY_ADDRESS << 1) | 0;
    
    TWI_WRITE_WAIT();
    
    sendCommandMonitor(0x35);
    sendCommandMonitor(0x17);
    
    //TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    TWI0.MCTRLB = TWI_MCMD_REPSTART_gc;
    
    TWI0.MADDR = (HUMIDITY_ADDRESS << 1) | 0;
    
    TWI_WRITE_WAIT();
    
    //TWI0.MDATA = HUMIDITY_ADDRESS;
    
    //TWI_WAIT();
    
    sendCommandMonitor(0x78);
    //TWI0.MCTRLB = TWI_MCMD_REPSTART_gc;
    sendCommandMonitor(0x66);
    
    //TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    //TWI0.MDATA = (HUMIDITY_ADDRESS);
    
    uint8_t count = 0;
    
    //_delay_ms(15000);
    //while(TWI0.MSTATUS & TWI_RXACK_bm){
        TWI0.MCTRLB = TWI_MCMD_REPSTART_gc;
        TWI0.MADDR = (HUMIDITY_ADDRESS<<1) | 1;
        //if((TWI0.MSTATUS&TWI_RXACK_bm))
          //  TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    //}
    
    
    while(count < len){
        TWI_WAIT();
        
        reading[count] = TWI0.MDATA;
                
        TWI_WAIT();
        
        count++;
        
        if (count != len)
        {
            //If not done, then ACK and read the next byte
            TWI0.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc;
        }
        PORTA.OUTCLR = PIN3_bm;
    }
    //TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
    TWI0.MCTRLB = TWI_MCMD_REPSTART_gc;
    TWI0.MADDR = (HUMIDITY_ADDRESS << 1) | 0;
    
    TWI_WRITE_WAIT();
    
    sendCommandMonitor(0xB0);
    sendCommandMonitor(0x98);
    
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    return reading;
}

void toggleColon(int on){
    TWI0.MADDR = (0x71 << 1) | 0;
    
    TWI_WAIT();

    TWI0.MDATA = 0x77;

    TWI_WAIT();
    
    if(on){
        TWI0.MDATA = 1 << 4;
    }
    else{
        TWI0.MDATA = 0 << 4;
    }
    
    TWI_WAIT();

    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

void printMessage(char* message, uint8_t len){
    TWI0.MADDR = (0x71 << 1) | 0;
    
    TWI_WAIT();
    
    uint8_t count = 0;
    
    while(count < len){
        TWI0.MDATA = message[count];
        
        TWI_WAIT();
        
        count++;
    }
    
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
}

uint8_t getHumidity(){
    //get humidity from sensor
    uint8_t reading[3];
    readMonitor(reading);
    return *reading;
}

void displayTitleNumber(char* title, int num){
    char message[4];
    sprintf(message, "%04d", num);
    printMessage(message, 4);
}

void sendCommandDisplay(uint8_t message){
    TWI0.MADDR = (DISPLAY_ADDRESS << 1) | 0;
    
    TWI_WAIT();

    TWI0.MDATA = message;

    TWI_WAIT();

    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
    
}

void sendCommandMonitor(uint8_t message){
    //TWI0.MADDR = (HUMIDITY_ADDRESS << 1) | 0;
    
    TWI_WAIT();

    TWI0.MDATA = message;

    TWI_WAIT();

    TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;
    
}

void writeTime(int first, int second){
    char message[4];
    sprintf(message, "%02d%02d", first, second);
    printMessage(message, 4);
}
