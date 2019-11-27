/* 
 * File:   utils.h
 * Author: Vasco & Sofia
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
#define DELAY               300 //ms
    
//default values
#define PMON                5 // monitoring period
#define NREG                30 // number of registers in the ring buffer
#define TALA                3 // duration of alarm signal (PWM)
#define ALAT                25 // threshold for temperature alarm
#define ALAL                2 // threshold for luminosity level alarm
#define ALAF                0 // alarm flag ? initially disabled
#define CLKH                0 // initial value for clock hours
#define CLKM                0 // initial value for clock minutes

    
extern bool s1flag; //flag que fica a true quando ha interrupao do S1
extern bool s2flag; //same mas para S2
extern uint8_t pmon; // monitoring period
extern uint8_t nreg; // number of data registers
extern uint8_t tala; // duration of alarm signal (PWM)
extern uint8_t alat; // threshold for temperature alarm
extern uint8_t alal; // threshold for luminosity level alarm
extern uint8_t alaf; // alarm flag ? initially disabled
extern volatile uint8_t clkh; // initial value for clock hours
extern volatile uint8_t clkm; // initial value for clock minutes
extern uint8_t illum;
extern uint8_t temp;
extern bool alarm;

uint8_t ReadIllum(void);
unsigned char ReadTemp(void);
void PWM_Output_D4_Enable (void);
void PWM_Output_D4_Disable (void);
void ShowOnLEDs(uint8_t);
void checkButtonS1(void);
void checkButtonS2(void);
void load_eeprom(void);
void default_setup(void);
void update_clk(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UTILS_H */

