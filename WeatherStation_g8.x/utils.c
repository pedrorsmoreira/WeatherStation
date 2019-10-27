#include "mcc_generated_files/mcc.h"
#include "I2C/i2c.h"
#include "utils.h"
//#include "eeprom.h"

int ReadIllum(void){
    return ADCC_GetSingleConversion(ILLUM)>> 14;
}

int ReadTemp(void){
	unsigned char value;
    return 10;
    do{
        IdleI2C();
        StartI2C()
                ; IdleI2C();

        WriteI2C(0x9a | 0x00); IdleI2C();
        WriteI2C(0x01); IdleI2C();
        RestartI2C()
                ; IdleI2C();
        WriteI2C(0x9a | 0x01); IdleI2C();
        value = ReadI2C(); IdleI2C();
        NotAckI2C()
                ; IdleI2C();
        StopI2C();
    } while (!(value & 0x40));

	IdleI2C();
	StartI2C()
            ; IdleI2C();
	WriteI2C(0x9a | 0x00); IdleI2C();
	WriteI2C(0x00); IdleI2C();
	RestartI2C()
            ; IdleI2C();
	WriteI2C(0x9a | 0x01); IdleI2C();
	value = ReadI2C(); IdleI2C();
	NotAckI2C()
            ; IdleI2C();
	StopI2C();

	return (int) value;
}

void PWM_Output_D4_Enable (void){
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS
    // Set D4 as the output of PWM6
    RA6PPS = 0x0E;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
}

void PWM_Output_D4_Disable (void){
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS
    // Set D4 as GPIO pin
    RA6PPS = 0x00;
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
}

void ShowOnLEDs(int num){
    LEDs = num << 4;
}

void checkButtonS1(void) {
    if (btn1State == false) {
        if (SWITCH1_PORT == LOW) {
            __delay_ms(100);
            btn1State = true;
        }
    } else if (SWITCH1_PORT == HIGH) {
        btn1State = false;
        s1flag = true;
    }
}

void checkButtonS2(void) {
    if (btn2State == false) {
        if (SWITCH2_PORT == LOW) {
            __delay_ms(100);
            btn2State = true;
        }
    } else if (SWITCH2_PORT == HIGH) {
        btn2State = false;
        s2flag = true;
    }
}


void eeprom_default_setup(void){
    eeprom_setup(true, NREG, PMON, TALA, 
            ALAT, ALAL, ALAF, CLKH, CLKM);
}

void update_clk(void){
    eeprom_clk_update(hours, minutes);
}

bool ring_buffer(void){
    return ring_buffer_write(hours, minutes, seconds, temp, illum);
}