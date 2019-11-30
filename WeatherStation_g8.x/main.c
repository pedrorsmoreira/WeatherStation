#include <stdio.h>

#include "mcc_generated_files/mcc.h"
#include "I2C/i2c.h"
#include "utils.h"
#include "menus.h"
#include "communications.h"

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
bool alarm;
volatile uint8_t msgs;
uint8_t num_msgs;
volatile uint8_t iread;

void sys_init(void){
    iread = 0;
    msgs = 0;
    num_msgs = 0;
    timer = 0;
    switch1 = false;
    flag_timer = false;
    initial_time = 0;
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

void Alarm(uint8_t h, uint8_t m, uint8_t s){
    alarm = true;
    PWM_Output_D4_Enable();
    PWM6_LoadDutyValue(PWM_MIN);
    initial_time = h * 3600 + m * 60 + s;
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
    EUSART_SetRxInterruptHandler(countMsg);
        
    i2c1_driver_open();
    I2C_SCL = 1;
    I2C_SDA = 1;
    WPUC3 = 1;
    WPUC4 = 1;
    
    EUSART_Write(0x57);
    putch(read_nreg());
    
    while (1)
    {           
        INTERRUPT_PeripheralInterruptDisable();
        if(flag_timer){
            flag_timer = false;
            update_clk();
        }
        
        hours = clkh; minutes = clkm; seconds = clks; // avoid conflicts in interrupt

        if(msgs) {
            num_msgs = msgs;
            msgs = 0;
        }

        if((timer == 0 || timer > pmon) && pmon != 0){
            timer = 1;
            INTERRUPT_PeripheralInterruptEnable();
            illum = ReadIllum();
            L0_LAT = illum & 1;
            L1_LAT = (illum & 2) >> 1;
            temp = ReadTemp();
            if(ring_buffer_write(hours, minutes, seconds, temp, illum))
                notification_memory();
            if((illum < alal || temp > alat) && alaf == 1){
                if(!alarm)
                    Alarm(hours, minutes, seconds);
            }
        } else
            INTERRUPT_PeripheralInterruptEnable();

        for(int i = 0; i < num_msgs; i++) {
            read_msgs();
        }
        num_msgs = 0;
        
        EXT_INT_InterruptDisable();
        if(alarm){
            EXT_INT_InterruptEnable();
            if(hours * 3600 + minutes * 60 + seconds - initial_time >= tala){
                PWM_Output_D4_Disable();
                A_SetHigh();
                // SLEEP();
            }
        } else {
            EXT_INT_InterruptEnable();
            // SLEEP();
        }

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