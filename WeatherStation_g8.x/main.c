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

#include "mcc_generated_files/mcc.h"
#include <stdio.h>
#include <stdlib.h>

/*
                         Main application
 */

uint8_t temp = 0;
uint8_t lum = 0;
uint8_t volatile timer = 0;
uint8_t volatile seconds = 0;
uint8_t volatile minutes = 0;
uint8_t volatile hours = 0;
uint8_t PMON = 5;


volatile bool timer1 = false;

void Timer(){
    C_Toggle();
    if(++timer >= PMON){
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

//////////////////////EEPROM START//////////////////////////////////////////

//Addresses
#define CHECK      0x7000 // EEPROM starting address; holds a checkup value
#define NREG       0x7001
#define PMON       0x7002
#define TALA       0x7003
#define ALAT       0x7004
#define ALAL       0x7005
#define ALAF       0x7006
#define CLKH       0x7007
#define CLKM       0x7008
#define WBUF       0x7009 //ring buffer current write position 
#define RBUF       0x700A // ring buffer starting position

uint8_t xor_eeprom(){
    uint8_t xor = DATAEE_ReadByte(NREG)^DATAEE_ReadByte(PMON)^DATAEE_ReadByte(TALA)^DATAEE_ReadByte(ALAT)
            ^DATAEE_ReadByte(ALAL)^DATAEE_ReadByte(ALAF)^DATAEE_ReadByte(CLKH)^DATAEE_ReadByte(CLKM);
    
    uint8_t wbuf = DATAEE_ReadByte(WBUF);
    uint8_t rbuf = DATAEE_ReadByte(RBUF);
    xor ^= wbuf^rbuf;
    
    while (rbuf < wbuf)
        xor ^= DATAEE_ReadByte(++rbuf);
    
    return xor;
}

void set_xor_eeprom(){
    uint8_t xor = xor_eeprom();
    DATAEE_WriteByte(CHECK, xor);
}

bool check_corruption(){
    uint8_t xor = DATAEE_ReadByte(CHECK);
    return xor_eeprom() ^ xor;
}

void eeprom_setup(bool reset_buffer, uint8_t nreg, uint8_t pmon, uint8_t tala, 
        uint8_t alat, uint8_t alal, uint8_t alaf, uint8_t clkh, uint8_t clkm){
    
    if (reset_buffer)
        DATAEE_WriteByte(WBUF, RBUF);
    
    DATAEE_WriteByte(NREG, nreg);
    DATAEE_WriteByte(PMON, pmon);
    DATAEE_WriteByte(TALA, tala);
    DATAEE_WriteByte(ALAT, alat);
    DATAEE_WriteByte(ALAL, alal);
    DATAEE_WriteByte(ALAF, alaf);
    DATAEE_WriteByte(CLKH, clkh);
    DATAEE_WriteByte(CLKM, clkm);
    set_xor_eeprom();
}

void eeprom_clk_update(uint8_t clkh, uint8_t clkm){
    DATAEE_WriteByte(CLKH, clkh);
    DATAEE_WriteByte(CLKM, clkm);
    set_xor_eeprom();
}

//returns true when it writes and false when it doesn't
bool ring_buffer_write(uint8_t h, uint8_t m, uint8_t s, uint8_t T, uint8_t L){
    
    uint8_t ring_pos = DATAEE_ReadByte(WBUF);
    
    if (T == DATAEE_ReadByte(ring_pos - 2) && L == DATAEE_ReadByte(ring_pos - 1)) 
        return false;
    
    if (ring_pos > (RBUF + DATAEE_ReadByte(NREG) - 5) )
        ring_pos = RBUF;
    
    DATAEE_WriteByte(ring_pos  , h);
    DATAEE_WriteByte(ring_pos+1, m);
    DATAEE_WriteByte(ring_pos+2, s);
    DATAEE_WriteByte(ring_pos+3, T);
    DATAEE_WriteByte(ring_pos+4, L);
    
    DATAEE_WriteByte(WBUF, ring_pos+5);
    
    set_xor_eeprom();
    
    return true;
}


//Defaults
#define defNREG    30
#define defPMON    5
#define defTALA    3
#define defALAT    25
#define defALAL    2
#define defALAF    0
#define defCLKH    0
#define defCLKM    0

void eeprom_default_setup(){
    eeprom_setup(true, defNREG, defPMON, defTALA, 
            defALAT, defALAL, defALAF, defCLKH, defCLKM);
}

void update_clk(){
    eeprom_clk_update(hours, minutes);
}

bool ring_buffer(){
    return ring_buffer_write(hours, minutes, seconds, temp, lum);
}

//////////////////////EEPROM END//////////////////////////////////////////




uint8_t x1;
uint8_t x2;
uint8_t x3;
uint8_t x4;
uint8_t x5;
uint8_t x6;
uint8_t h;
uint8_t m;




void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    TMR1_SetInterruptHandler(Timer);

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    eeprom_default_setup();
    
    x1 = DATAEE_ReadByte(NREG);
    x2 = DATAEE_ReadByte(PMON);
    x3 = DATAEE_ReadByte(TALA);
    x4 = DATAEE_ReadByte(ALAT);
    x5 = DATAEE_ReadByte(ALAL);
    x6 = DATAEE_ReadByte(ALAF);
    
    DATAEE_WriteByte(CLKH, 3);
    DATAEE_WriteByte(CLKM, 4);
    
    h = DATAEE_ReadByte(CLKH);
    m = DATAEE_ReadByte(CLKM);
    
    while (1)
    {
        h = DATAEE_ReadByte(CLKH);
        m = DATAEE_ReadByte(CLKM);
        NOP();
        DATAEE_WriteByte(CLKH, x2++);
        DATAEE_WriteByte(CLKM, x3++);
        //if(timer1){
        //    timer1 = false;
            //read 
        //}
    }
}

/**
 End of File
*/