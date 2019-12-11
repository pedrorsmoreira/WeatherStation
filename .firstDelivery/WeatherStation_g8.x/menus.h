/* 
 * File:   menus.h
 * Author: Vasco & Sofia
 *
 * Created on October 25, 2019, 2:56 PM
 */

#ifndef MENUS_H
#define	MENUS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "utils.h"
    

volatile uint8_t mode = 0;

void Menu(uint8_t mode);
uint8_t Update(uint8_t var, uint8_t thr);
void submenu_clock(void);
void submenu_alarm(void);
void submenu_temp(void);
void submenu_illum(void);
void Menus(void);
void Blink(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MENUS_H */

