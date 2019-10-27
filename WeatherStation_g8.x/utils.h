/* 
 * File:   utils.h
 * Author: vasco
 *
 * Created on October 25, 2019, 2:57 PM
 */

#ifndef UTILS_H
#define	UTILS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"
#include "I2C/i2c.h"
#include "utils.h"
#include "eeprom.h"
    
#define LEDs                LATA
    
    
extern int volatile seconds;
extern int volatile minutes;
extern int volatile hours;
bool btn1State = false;
bool btn2State = false;
extern bool s1flag; //flag que fica a true quando ha interrupao do S1
extern bool s2flag; //same mas para S2
extern int PMON; // monitoring period
extern int NREG; // number of data registers
extern int TALA; // duration of alarm signal (PWM)
extern int ALAT; // threshold for temperature alarm
extern int ALAL; // threshold for luminosity level alarm
extern int ALAF; // alarm flag ? initially disabled
extern int CLKH; // initial value for clock hours
extern int CLKM; // initial value for clock minutes

extern int illum;
extern int temp;

int ReadIllum(void);
int ReadTemp(void);
void PWM_Output_D4_Enable (void);
void PWM_Output_D4_Disable (void);
void ShowOnLEDs(int);
void checkButtonS1(void);
void checkButtonS2(void);

void eeprom_default_setup(void);
void update_clk(void);
bool ring_buffer(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_H */

