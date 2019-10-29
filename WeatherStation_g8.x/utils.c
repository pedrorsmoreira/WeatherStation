#include "mcc_generated_files/mcc.h"
#include "I2C/i2c.h"
#include "utils.h"

bool s1flag;
bool s2flag;
uint8_t nreg;
bool btn1State;
bool btn2State;

uint8_t ReadIllum(void){
    return ADCC_GetSingleConversion(ILLUM)>> 14;
}

unsigned char ReadTemp(void){
	unsigned char value;

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

	return value;
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

void ShowOnLEDs(uint8_t num){
    LEDs = num << 4;
}

void checkButtonS1(void) {
    if (btn1State == false) {
        if (SWITCH1_PORT == LOW) {
            __delay_ms(DELAY);
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
            __delay_ms(DELAY);
            btn2State = true;
        }
    } else if (SWITCH2_PORT == HIGH) {
        btn2State = false;
        s2flag = true;
    }
}

void load_eeprom(void){
  //  if(used() && check_corruption()){
    if(used()){
        pmon = read_pmon(); // monitoring period
        nreg = read_nreg(); // number of data registers
        tala = read_tala(); // duration of alarm signal (PWM)
        alat = read_alat(); // threshold for temperature alarm
        alal = read_alal(); // threshold for luminosity level alarm
        alaf = read_alaf(); // alarm flag ? initially disabled
        clkh = read_clkh(); // initial value for clock hours
        clkm = read_clkm(); // initial value for clock minutes
    } else
        default_setup();
}

void default_setup(void){
    pmon = PMON;
    nreg = NREG;
    tala = TALA;
    alat = ALAT;
    alal = ALAL;
    alaf = ALAF;
    clkh = CLKH;
    clkm = CLKM;
    eeprom_setup(true, NREG, PMON, TALA, 
            ALAT, ALAL, ALAF, CLKH, CLKM);
}

void update_clk(void){
    eeprom_clk_update(clkh, clkm);
}

bool ring_buffer(void){
    return ring_buffer_write(clkh, clkm, seconds, temp, illum);
}