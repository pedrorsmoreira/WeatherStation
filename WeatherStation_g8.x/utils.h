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
    
    
extern uint8_t volatile seconds;
bool btn1State;
bool btn2State;
extern bool s1flag; //flag que fica a true quando ha interrupao do S1
extern bool s2flag; //same mas para S2
extern uint8_t PMON; // monitoring period
extern uint8_t NREG; // number of data registers
extern uint8_t TALA; // duration of alarm signal (PWM)
extern uint8_t ALAT; // threshold for temperature alarm
extern uint8_t ALAL; // threshold for luminosity level alarm
extern uint8_t ALAF; // alarm flag ? initially disabled
extern volatile uint8_t CLKH; // initial value for clock hours
extern volatile uint8_t CLKM; // initial value for clock minutes

extern uint8_t illum;
extern uint8_t temp;

uint8_t ReadIllum(void);
uint8_t ReadTemp(void);
void PWM_Output_D4_Enable (void);
void PWM_Output_D4_Disable (void);
void ShowOnLEDs(uint8_t);
void checkButtonS1(void);
void checkButtonS2(void);
void load_eeprom(void);
void eeprom_default_setup(void);
void update_clk(void);
bool ring_buffer(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_H */

