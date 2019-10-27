#include "menus.h"
#include "utils.h"
#include "mcc_generated_files/mcc.h"

// funcao que dita o menu dentro do modificaction mode
void Menu(uint8_t mode) {
    TMR1_StopTimer();
	switch(mode){
		case 0:
			submenu_clock();
			break;
		case 1:
			submenu_alarm();
			break;
		case 2:
			submenu_temp();
			break;
		case 3:
			submenu_illum();
			break;
	}
    TMR1_StartTimer();
    ShowOnLEDs(0);
}

// funcao generica de update do valor
uint8_t Update(uint8_t var, uint8_t thr){
	ShowOnLEDs(var); //funcao de escrita do digito em binario nos leds
	while(true){
        checkButtonS1();
        checkButtonS2();
		if (s2flag){
			s2flag = false;
			if (++var > thr)
				var=0;
            ShowOnLEDs(var);
		}
		else if (s1flag){
			s1flag = false;
			return var;
		}
	}
}

//FUNCOES DE CADA SUBMENU
void submenu_clock(void){
	uint8_t hours_tens = CLKH/10; //hours tens 
	uint8_t hours_units = CLKH%10;  //hours units
	hours_tens = Update(hours_tens, (uint8_t) 2);
	if(hours_tens == 2){
		if(hours_units>3) hours_units = 0;
		hours_units = Update(hours_units, (uint8_t) 3);
	} else 
		hours_units = Update(hours_units, (uint8_t) 9);
	
	uint8_t minutes_tens = CLKM/10; // minutes tens
	uint8_t minutes_units = CLKM/10; //minutes units
	minutes_tens = Update(minutes_tens, (uint8_t) 5);
	minutes_units = Update(minutes_units, (uint8_t) 9);
	
	CLKH = 10*hours_tens + hours_units;
	CLKM = 10*minutes_tens + minutes_units;
    update_clk();
}



void submenu_alarm(void){
	ALAF = Update(ALAF, (uint8_t) 1);
}

void submenu_temp(void){
	uint8_t temperature_tens = ALAT/10; //temperature tens
	uint8_t temperature_units = ALAT % 10; //temperature units
	temperature_tens = Update(temperature_tens, (uint8_t) 5);
	if (temperature_tens == 5) ALAT = 50;
	else {
		temperature_units = Update(temperature_units, (uint8_t) 9);
		ALAT = temperature_tens*10 + temperature_units;
	}
}

void submenu_illum(void){
	ALAL = Update(ALAL, (uint8_t) 3);
}


void Menus (void) {
    mode = 0;
    ShowOnLEDs(0);
    TMR1_SetInterruptHandler(Blink);
    
    while(mode <= 3){
        checkButtonS1();
        checkButtonS2();
        if (s1flag){
            s1flag = false;
            mode++;
            ShowOnLEDs(0);
            //blink_LED(ledslista[i]);
        } else if (s2flag) {
            s2flag = false;
            ShowOnLEDs(0);
        //	stop_blink(ledslista[i]); //para o led de piscar
            Menu(mode++);
        }
	}    
}

void Blink(void){
    switch(mode){
        case 0:
            C_Toggle();
            break;
        case 1:
            A_Toggle();
            break;
        case 2:
            L1_Toggle();
            break;
        case 3:
            L0_Toggle();
            break;
    }
}
