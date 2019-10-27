/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC16F18875
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

//#include "mcc_generated_files/mcc.h"
//#include "I2C/i2c.h"
//#include "utils.h"
#include "menus.h"

#define PWM_MIN     250
#define PWM_MAX     1023

/*
                         Main application
 */


int volatile seconds = 0;
int volatile minutes = 0;
int volatile hours = 0;
bool s1flag = false; //flag que fica a true quando ha interrupao do S1
bool s2flag = false; //same mas para S2
int PMON = 5; // monitoring period
int NREG = 30; // number of data registers
int TALA = 3; // duration of alarm signal (PWM)
int ALAT = 25; // threshold for temperature alarm
int ALAL = 2; // threshold for luminosity level alarm
int ALAF = 0; // alarm flag ? initially disabled
int CLKH = 0; // initial value for clock hours
int CLKM = 0; // initial value for clock minutes


int volatile timer = 0;
float volatile pwm = 0;
bool volatile switch1 = false;
int initial_time = 0;

bool alarm = false;
int illum = -1;
int temp = -1;
float incr;

volatile bool timer1 = false;

void Timer(){
    C_Toggle();
    if(timer++ >= PMON){
        timer = 0;
        timer1 = true;
    }

    if(seconds < 59)
        seconds++;
    else{
        seconds = 0;
        if(minutes < 59)
            minutes++;
        else{
            minutes = 0;
            if(hours < 23)
                hours++;
            else
                hours = 0;
        }
    }
}

void ClearAlarm(){
    PWM_Output_D4_Disable();
    TMR2_Stop();
    pwm = 0;
    alarm = false;
}

void Switch1(void){
    if(alarm)
        ClearAlarm();
    else
        switch1 = true;
}

/*
void Timer2(void){
    if(pwm < 1023-incr)
        pwm += incr;
    else{
        pwm = 1023;
        TMR2_Stop();
    }
}
*/

void Alarm(void){
    alarm = true;
    PWM_Output_D4_Enable();
    PWM6_LoadDutyValue(PWM_MIN);
    TMR2_StartTimer();
    initial_time = TMR2_ReadTimer();
}

uint8_t xor(uint8_t x, uint8_t y){
    return x^y;
}

void main(void)
{
    set_check_up_value(xor);
    
    // initialize the device
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    TMR1_SetInterruptHandler(Timer);
   // TMR2_SetInterruptHandler(Timer2);

  //  incr = 1023*1.024/(1000*TALA);
    INT_SetInterruptHandler(Switch1);

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
        
    i2c1_driver_open();
    I2C_SCL = 1;
    I2C_SDA = 1;
    WPUC3 = 1;
    WPUC4 = 1;

    while (1)
    {   
        if(timer1){
            timer1 = false;
            illum = ReadIllum();
            L0_LAT = illum & 1;
            L1_LAT = (illum & 2) >> 1;
            temp = ReadTemp();
            if(illum < ALAL)
                if(!alarm) Alarm();
        }
        
        if(alarm)
            if(TMR1_ReadTimer()-initial_time > TALA)
                    PWM6_LoadDutyValue(PWM_MAX);
        
        EXT_INT_InterruptDisable();
        if(switch1){
            switch1 = false;
            s1flag = false;
            SWITCH1_SetHigh();
            Menus();
            TMR1_SetInterruptHandler(Timer);
        }
        EXT_INT_InterruptEnable();
    }
}

/**
 End of File
*/