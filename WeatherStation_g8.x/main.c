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

bool s1flag; //flag que fica a true quando ha interrupao do S1
bool s2flag; //same mas para S2


int volatile timer;
bool volatile switch1;
bool volatile flag_timer;
int initial_time;
uint8_t PMON = 5; // monitoring period
uint8_t NREG = 30; // number of data registers
uint8_t TALA = 3; // duration of alarm signal (PWM)
uint8_t ALAT = 25; // threshold for temperature alarm
uint8_t ALAL = 2; // threshold for luminosity level alarm
uint8_t ALAF = 0; // alarm flag ? initially disabled
volatile uint8_t CLKH = 0; // initial value for clock hours
volatile uint8_t CLKM = 0; // initial value for clock minutes
uint8_t temp;
uint8_t illum;
volatile uint8_t seconds;

bool alarm;
float incr;

volatile bool timer1 = false;

void Timer(){
    C_Toggle();
    
    if(PMON == 0)
        return;
    
    if(timer++ >= PMON){
        timer = 0;
        timer1 = true;
    }

    if(seconds < 59)
        seconds++;
    else{
        seconds = 0;
        if(CLKM < 59)
            CLKM++;
        else{
            CLKM = 0;
            if(CLKH < 23)
                CLKH++;
            else
                CLKH = 0;
        }
    }
    
    if(seconds == 0){
        flag_timer = true;
    }
}

void ClearAlarm(){
    PWM_Output_D4_Disable();
    A_SetLow();
    //TMR2_Stop();
    alarm = false;
}

void Switch1(void){
    if(alarm)
        ClearAlarm();
    else
        switch1 = true;
}

void Alarm(void){
    alarm = true;
    PWM_Output_D4_Enable();
    PWM6_LoadDutyValue(PWM_MIN);
    TMR2_StartTimer();
    initial_time = CLKH * 3600 + CLKM * 60 + seconds;
}

uint8_t xor(uint8_t x, uint8_t y){
    return x^y;
}

void main(void)
{
    
    timer = 0;
    switch1 = false;
    flag_timer = false;
    initial_time = 0;
    btn1State = false;
    btn2State = false;
    alarm = false;

    /*
    #ifdef CHECKSUM
    set_check_up_value();
    #else
    set_check_up_value(xor);
    #endif
    */
    
    // initialize the device
    SYSTEM_Initialize();

    load_eeprom();
    ClearAlarm();
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    TMR1_SetInterruptHandler(Timer);
    INT_SetInterruptHandler(Switch1);
   // TMR2_SetInterruptHandler(Timer2);

  //  incr = 1023*1.024/(1000*TALA);

    // Disable the Global Interrupts
  //  INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
        
    i2c1_driver_open();
    I2C_SCL = 1;
    I2C_SDA = 1;
    WPUC3 = 1;
    WPUC4 = 1;

    while (1)
    {           
        INTERRUPT_PeripheralInterruptDisable();
        if(flag_timer){
            flag_timer = false;
            update_clk();
        }
        if(timer1){
            timer1 = false;
            INTERRUPT_PeripheralInterruptEnable();
            illum = ReadIllum();
            L0_LAT = illum & 1;
            L1_LAT = (illum & 2) >> 1;
            temp = ReadTemp();
            ring_buffer();
            if((illum < ALAL || temp > ALAT) && ALAF == 1)
                if(!alarm) Alarm();
        } else
            INTERRUPT_PeripheralInterruptEnable();
        
        if(alarm){
            if(CLKH * 3600 + CLKM * 60 + seconds - initial_time >= TALA){
                PWM_Output_D4_Disable();
                A_SetHigh();
                SLEEP();
            }
        } else
            SLEEP();
        
        EXT_INT_InterruptDisable();
        if(switch1){
            switch1 = false;
            s1flag = false;
            SWITCH1_SetHigh();
            Menus();
            INTERRUPT_PeripheralInterruptDisable();
            TMR1_SetInterruptHandler(Timer);
            INTERRUPT_PeripheralInterruptEnable();
            EXT_INT_InterruptFlagClear();

        }
        EXT_INT_InterruptEnable();
    }
}

/**
 End of File
*/