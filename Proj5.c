/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
#pragma config JTAGEN = OFF     // Turn off JTAG - required to use Pin RA0 as IO
#pragma config FNOSC = PRIPLL   //configure system clock 80 MHz
#pragma config FSOSCEN = OFF    // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT     // Primary Oscillator Configuration (XT osc mode)
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_1
#pragma config FPBDIV = DIV_2   //configure peripheral bus clock to 40 MHz

#ifndef _SUPPRESS_PLIB_WARNING
#define _SUPPRESS_PLIB_WARNING
#endif
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
#include <xc.h>
#include <stdio.h>
#include <plib.h>
#include "config.h"
#include <math.h>
#include "adc.h"
#include "btn.h"
#include "lcd.h"
#include "pmods.h"
#include "rgbled.h"
#include "srv.h"
#include "ssd.h"
#include "swt.h"
#include "uart.h"
#include "ultr.h"
#include "utils.h"

#define SYS_FREQ (80000000L)
#define INTSEC 10
#define CORE_TICK_RATE (SYS_FREQ / 2 / INTSEC)

int counter = 0;
int stoppedLeft = 0;
int stoppedRight = 0;

int clockwise = 900;
int cClockwise = 2100;
int stop = 1500;

main (void){
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    OpenCoreTimer(CORE_TICK_RATE); //CoreTimer used for tenths of second capture
    mConfigIntCoreTimer((CT_INT_ON | CT_INT_PRIOR_5 | CT_INT_SUB_PRIOR_0));
    INTEnableSystemMultiVectoredInt(); //Enable Timer interrupts
    ADC_Init();
    delay_ms(100);
    BTN_Init();
    delay_ms(100);
    LCD_Init();
    delay_ms(100);
    RGBLED_Init();
    delay_ms(100);
    SRV_Init();
    delay_ms(100);
    SSD_Init();
    delay_ms(100);
    SWT_Init();
    delay_ms(100);
    ULTR_Init(0,2,0,3);
    delay_ms(100);
    LED_Init();
    delay_ms(100);
    LCD_WriteStringAtPos("Tm:1 Beebo 3:2",0,0);
    char swtRight[2];
    char swtLeft[2];
    while(1){
        swtRight[0] = SWT_GetValue(0);
        swtRight[1] = SWT_GetValue(1);
        swtLeft[0] = SWT_GetValue(6);
        swtLeft[1] = SWT_GetValue(7);
        //check sw0 and sw0
        if(swtRight[0]==0 && swtRight[1]==0){//stop
            LCD_WriteStringAtPos("STP",1,10);
            stoppedRight = 1;
            SRV_SetPulseMicroseconds0(stop);
            LED_SetValue(3,0);
            LED_SetValue(2,0);
            LED_SetValue(1,0);
            LED_SetValue(0,0);
        }
        else if(swtRight[0]==1 && swtRight[1]==0){//forward
            LCD_WriteStringAtPos("FWD",1,10);
            stoppedRight = 0;
            SRV_SetPulseMicroseconds0(clockwise);
            LED_SetValue(3,1);
            LED_SetValue(2,1);
            LED_SetValue(1,0);
            LED_SetValue(0,0);
        }
        else if(swtRight[0]==0 && swtRight[1]==1){//reverse
            LCD_WriteStringAtPos("REV",1,10);
            stoppedRight = 0;
            SRV_SetPulseMicroseconds0(cClockwise);
            LED_SetValue(3,0);
            LED_SetValue(2,0);
            LED_SetValue(1,1);
            LED_SetValue(0,1);
        }
        else if(swtRight[0]==1 && swtRight[1]==1){//stop
            LCD_WriteStringAtPos("STP",1,10);
            stoppedRight = 1;
            SRV_SetPulseMicroseconds0(stop);
            LED_SetValue(3,0);
            LED_SetValue(2,0);
            LED_SetValue(1,0);
            LED_SetValue(0,0);
        }
        //check sw6 and sw7
        if(swtLeft[0]==0 && swtLeft[1]==0){//stop
            LCD_WriteStringAtPos("STP",1,0);
            stoppedLeft = 1;
            SRV_SetPulseMicroseconds1(stop);
            LED_SetValue(7,0);
            LED_SetValue(6,0);
            LED_SetValue(5,0);
            LED_SetValue(4,0);
        }
        else if(swtLeft[0]==1 && swtLeft[1]==0){//forward
            LCD_WriteStringAtPos("FWD",1,0);
            stoppedLeft = 0;
            SRV_SetPulseMicroseconds1(cClockwise);
            LED_SetValue(7,0);
            LED_SetValue(6,0);
            LED_SetValue(5,1);
            LED_SetValue(4,1);
        }        
        else if(swtLeft[0]==0 && swtLeft[1]==1){//reverse
            LCD_WriteStringAtPos("REV",1,0);
            stoppedLeft = 0;
            SRV_SetPulseMicroseconds1(clockwise);
            LED_SetValue(7,1);
            LED_SetValue(6,1);
            LED_SetValue(5,0);
            LED_SetValue(4,0);
        }
        else if(swtLeft[0]==1 && swtLeft[1]==1){//stop
            LCD_WriteStringAtPos("STP",1,0);
            stoppedLeft = 1;
            SRV_SetPulseMicroseconds1(stop);
            LED_SetValue(7,0);
            LED_SetValue(6,0);
            LED_SetValue(5,0);
            LED_SetValue(4,0);
        }        
    }
    
}

void __ISR(_CORE_TIMER_VECTOR, ipl5) _CoreTimerHandler(void){
    mCTClearIntFlag();
    if(stoppedLeft==0 || stoppedRight==0){
        update_SSD(counter);
        counter++;
    }
    else if(stoppedLeft==1 && stoppedRight==1){
        counter=0;
        update_SSD(counter);
    }
    UpdateCoreTimer(CORE_TICK_RATE);
}

void delay_ms(int ms) {
    int i, counter;
    for (counter = 0; counter < ms; counter++) {
        for (i = 0; i < 300; i++) {
        } //software delay ~1 millisec 
    }
}

void update_SSD(int value) {
    int hunds, tens, ones, tenths;
    int dec1, dec2;
    char SSD1 = 0b0000000; //SSD setting for 1st SSD (LSD)
    char SSD2 = 0b0000000; //SSD setting for 2nd SSD
    char SSD3 = 0b0000000; //SSD setting for 3rd SSD
    char SSD4 = 0b0000000; //SSD setting for 4th SSD (MSD)
    if (value < 0) {
        SSD4 = 17;
        value = -1 * value;
        dec1 = 0;
        dec2 = 1;
    } else {
        dec1 = 1;
        dec2 = 0;
        hunds = floor(value / 1000);
        if (hunds > 0)
            SSD4 = hunds; //SSD4 = display_char[thous];
        else
            SSD4 = 0;
    }
    tens = floor((value % 1000) / 100);
    if (hunds == 0 && tens == 0)
        SSD3 = 0;
    else
        SSD3 = tens;
    SSD2 = ones = floor(value % 100 / 10);
    SSD1 = tenths = floor(value % 10);
    SSD_WriteDigits(SSD1, SSD2, SSD3, SSD4, 0, 0, dec2, dec1);
}