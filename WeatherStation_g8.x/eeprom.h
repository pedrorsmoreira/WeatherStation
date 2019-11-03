/* 
 * File:   eeprom.h
 * Author: Pedro
 *
 * Created on October 25, 2019, 2:57 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

//calculate the chekup value by summing all the eeprom values
#define CHECKSUM

#include "mcc_generated_files/mcc.h"

//Addresses
#define CHECK_      0xF000 // EEPROM starting address; holds a corruption checkup value
#define NREG_       0xF001
#define PMON_       0xF002
#define TALA_       0xF003
#define ALAT_       0xF004
#define ALAL_       0xF005
#define ALAF_       0xF006
#define CLKH_       0xF007 
#define CLKM_       0xF008
#define USED_       0xF009 //to check if the eeprom was already set  (if so, it holds th value 0xAA)
#define WBUF_       0xF00A //ring buffer current write position 
#define RBUF_       0xF00B // ring buffer starting position


//returns the computed check_up_value
#ifdef CHECKSUM
uint8_t get_check_up_value(void);
#else
uint8_t get_check_up_value( uint8_t (*func) (uint8_t, uint8_t));
#endif
//sets in the eeprom the computed check_up_value
#ifdef CHECKSUM
void set_check_up_value(void);
#else
void set_check_up_value( uint8_t (*func) (uint8_t, uint8_t));
#endif
//check if the eeprom check_up_value matches the computed one
#ifdef CHECKSUM
bool check_corruption(void);
#else
bool check_corruption( uint8_t (*func) (uint8_t, uint8_t));
#endif


//initialize the eeprom values
void eeprom_setup(bool reset_buffer, uint8_t nreg, uint8_t pmon, uint8_t tala, 
        uint8_t alat, uint8_t alal, uint8_t alaf, uint8_t clkh, uint8_t clkm);
//update the eeprom clock values (to be done every minute)
void eeprom_clk_update(uint8_t clkh, uint8_t clkm);
//returns true when it writes and false when it doesn't
bool ring_buffer_write(uint8_t h, uint8_t m, uint8_t s, uint8_t T, uint8_t L);
//returns if the eeprom was already used
bool used(void);


//basic write/read
uint8_t read_nreg(void);
uint8_t read_pmon(void);
uint8_t read_tala(void);
uint8_t read_alat(void);
uint8_t read_alal(void);
uint8_t read_alaf(void);
uint8_t read_clkh(void);
uint8_t read_clkm(void);
void write_nreg(uint8_t x);
void write_pmon(uint8_t x);
void write_tala(uint8_t x);
void write_alat(uint8_t x);
void write_alal(uint8_t x);
void write_alaf(uint8_t x);


#endif	/* EEPROM_H */


