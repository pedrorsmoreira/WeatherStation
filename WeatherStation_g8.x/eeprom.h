/* 
 * File:   eeprom.h
 * Author: pedro
 *
 * Created on October 25, 2019, 2:57 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

//calculate the chekup value by summing all the eeprom values
#define CHECKSUM

#include "mcc_generated_files/mcc.h"

//Addresses
#define CHECK_      0x7000 // EEPROM starting address; holds a corruption checkup value
#define NREG_       0x7001
#define PMON_       0x7002
#define TALA_       0x7003
#define ALAT_       0x7004
#define ALAL_       0x7005
#define ALAF_       0x7006
#define CLKH_       0x7007
#define CLKM_       0x7008
#define USED_       0x7009 //to check if the eeprom was already set  (if so, it holds th value 0xAA)
#define WBUF_       0x700A //ring buffer current write position 
#define RBUF_       0x700B // ring buffer starting position


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
void write_nreg(uint8_t x);
void write_pmon(uint8_t x);
void write_tala(uint8_t x);
void write_alat(uint8_t x);
void write_alal(uint8_t x);
void write_alaf(uint8_t x);


#endif	/* EEPROM_H */


