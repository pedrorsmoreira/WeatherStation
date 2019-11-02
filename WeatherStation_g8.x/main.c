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

volatile uint8_t timer;
volatile bool switch1;
volatile bool flag_timer;
int initial_time;
uint8_t pmon; // monitoring period
uint8_t tala; // duration of alarm signal (PWM)
uint8_t alat; // threshold for temperature alarm
uint8_t alal; // threshold for luminosity level alarm
uint8_t alaf; // alarm flag ? initially disabled
volatile uint8_t clkh; // initial value for clock hours
volatile uint8_t clkm; // initial value for clock minutes
volatile uint8_t clks;
uint8_t hours;
uint8_t minutes;
uint8_t seconds;
uint8_t temp;
uint8_t illum;
bool btn1State;
bool btn2State;

bool alarm;
float incr;

void sys_init(void){
    timer = 0;
    switch1 = false;
    flag_timer = false;
    initial_time = 0;
    btn1State = false;
    btn2State = false;
    alarm = false;
}

void Timer(){
    C_Toggle();
    
    if(pmon != 0)
        timer++;

    if(clks < 59)
        clks++;
    else{
        clks = 0;
        if(clkm < 59)
            clkm++;
        else{
            clkm = 0;
            if(clkh < 23)
                clkh++;
            else
                clkh = 0;
        }
    }
    
    if(clks == 0){
        flag_timer = true;
    }
}

void clear_alarm(){
    PWM_Output_D4_Disable();
    A_SetLow();
    alarm = false;
}

void Switch1(void){
    if(alarm)
        clear_alarm();
    else
        switch1 = true;
}

void Alarm(void){
    alarm = true;
    PWM_Output_D4_Enable();
    PWM6_LoadDutyValue(PWM_MIN);
    initial_time = clkh * 3600 + clkm * 60 + clks;
}

uint8_t xor(uint8_t x, uint8_t y){
    return x^y;
}

void main(void)
{
   
    // initialize the device
    SYSTEM_Initialize();
    sys_init();
    load_eeprom();
    clear_alarm();
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    TMR1_SetInterruptHandler(Timer);
    INT_SetInterruptHandler(Switch1);
        
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
        
        hours = clkh; minutes = clkm; seconds = clks; // avoid conflicts in interrupt

        if((timer == 0 || timer > pmon) && pmon != 0){
            timer = 1;
            INTERRUPT_PeripheralInterruptEnable();
            illum = ReadIllum();
            L0_LAT = illum & 1;
            L1_LAT = (illum & 2) >> 1;
            temp = ReadTemp();
            ring_buffer_write(hours, minutes, seconds, temp, illum);
            if((illum < alal || temp > alat) && alaf == 1){
                if(!alarm)
                    Alarm();
            }
        } else
            INTERRUPT_PeripheralInterruptEnable();
        
        if(alarm){
            if(hours * 3600 + minutes * 60 + seconds - initial_time >= tala){
                PWM_Output_D4_Disable();
                A_SetHigh();
                SLEEP();
            }
        } else
            SLEEP();
        
        EXT_INT_InterruptDisable();
        if(switch1){
            switch1 = false;
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