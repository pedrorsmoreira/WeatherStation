#include <string.h>
#include <stdio.h>

#include "eeprom.h"
#include "utils.h"
#include "communications.h"
#include "mcc_generated_files/eusart.h"
#include "mcc_generated_files/mcc.h"

volatile uint8_t clkh;
volatile uint8_t clkm;
volatile uint8_t clks;
uint8_t temp;
uint8_t illum;
uint8_t pmon; // monitoring period
uint8_t tala; // duration of alarm signal (PWM)
uint8_t alat; // threshold for temperature alarm
uint8_t alal; // threshold for luminosity level alarm
uint8_t alaf; // alarm flag ? initially disabled
uint8_t pmon; // monitoring period
uint8_t tala; // duration of alarm signal (PWM)
bool alarm;
volatile uint8_t msgs;
volatile uint8_t iread;
bool overlaped = false;
uint8_t not_transferred = 0;
uint8_t counter = 0;

void read_msgs(void) {
    char aux[8];
    uint8_t cmd = 0;
    int i = 0;
   
    do {
        if(!EUSART_is_rx_ready()) break;
        cmd = EUSART_Read();
    } while (cmd != SOM && EUSART_is_rx_ready());

    for(i = 0; cmd != EOM && EUSART_is_rx_ready(); i++) {
        cmd = EUSART_Read();
        aux[i] = cmd;
    }
    
    if(aux[i-1] == EOM) interpret_message(aux, i-2);
}

void interpret_message(char * msg, int size) {
    uint8_t cmd = msg[0];
    char * data = msg + 1;
        
    switch(cmd) {
        case RCLK:
            read_clock();
            break;
        case SCLK:
            set_clock(data, size);
            break;
        case RTL:
            read_temp_lum();
            break;
        case RPAR:
            read_param();
            break;
        case MMP:
            modify_pmon(data, size);
            break;
        case MTA:
            modify_time_alarm(data, size);
            break;
        case RALA:
            read_alarms();
            break;
        case DATL:
            define_alarm(data, size);
            break;
        case AALA:
            switch_alarm(data, size);
            break;
        case IREG:
            registers_info();
            break;
        case TRGC:
            transfer_n_registers(data, size);
            break;
        case TRGI:
            transfer_registers_i(data, size);
            break;
        default:
            send_confirmation(cmd, CMD_ERROR);
            break;
    }
}

void read_clock(void) {
    INTERRUPT_PeripheralInterruptDisable();
    uint8_t hours = clkh, minutes = clkm, seconds = clks; // avoid conflicts in interrupt
    INTERRUPT_PeripheralInterruptEnable();
    char msg[] = {RCLK, hours, minutes, seconds};
    send_msg(msg, 4);
}

void set_clock(char * data, int size) {    
//    if(size != 3 || data[0] > 23 || data[1] > 59 || data[2] > 59) {
    if(data[0] > 23 || data[1] > 59 || data[2] > 59) {
        send_confirmation(SCLK, CMD_ERROR);
        return;
    }
    INTERRUPT_PeripheralInterruptDisable();
    clkh = data[0]; clkm = data[1]; clks = data[2]; // avoid conflicts in interrupt
    update_clk();
    INTERRUPT_PeripheralInterruptEnable();
    send_confirmation(SCLK, CMD_OK);
}

void read_temp_lum(void) {
    char msg[] = {RTL, temp, illum};
    send_msg(msg, 3);
}

void read_param(void) {
    char msg[] = {RPAR, pmon, tala};
    send_msg(msg, 3);
}

void modify_pmon(char * data, int size) {
//    if(size != 1 || data[0] > 99) {
    if(data[0] > 99) {
        send_confirmation(MMP, CMD_ERROR);
        return;
    }
    pmon = data[0];
    write_pmon(pmon);
    send_confirmation(MMP, CMD_OK);
}

void modify_time_alarm(char * data, int size) {
//    if(size != 1 || data[0] > 60) {
    if(data[0] > 60) {
        send_confirmation(MTA, CMD_ERROR);
        return;
    }
    tala = data[0];
    write_tala(tala);
    send_confirmation(MTA, CMD_OK);
}

void read_alarms(void) {
    char msg[] = {RALA, alat, alal, alaf};
    send_msg(msg, 4);
}

void define_alarm(char * data, int size) {
//    if(size != 2 || data[0] > 50 || data[1] > 3) {
    if(data[0] > 50 || data[1] > 3) {
        send_confirmation(DATL, CMD_ERROR);
        return;
    }
    alat = data[0]; alal = data[1];
    send_confirmation(DATL, CMD_OK);
}

void switch_alarm(char * data, int size) {
//    if(size != 1 || data[0] > 1) {
    if(data[0] > 1) {
        send_confirmation(AALA, CMD_ERROR);
        return;
    }
    alaf = data[0];
    write_alaf(alaf);
    send_confirmation(AALA, CMD_OK);
}

void registers_info(void) {
    uint8_t N = read_nreg(), wi = read_iwrt(), nr = ring_buffer_laped() ? N : wi, ri = iread;
    char msg[] = {IREG, N, nr, ri, wi};
    send_msg(msg, 5);
}

void transfer_n_registers(char * data, int size) {
    uint8_t nr = ring_buffer_laped() ? read_nreg() : read_iwrt(), ri = iread, n;
//    if(size != 1 ||  data[0] > nr) {
    if(data[0] > nr) {
        send_confirmation(TRGC, CMD_ERROR);
        return;
    }
    putch(SOM);
    putch(TRGC);
    if(!transfer_registers(data[0], iread, -1))
        putch(CMD_ERROR);
    else putch(EOM);
}

void transfer_registers_i(char * data, int size) {
    uint8_t N = read_nreg(), wi = read_iwrt(), nr = ring_buffer_laped() ? N : wi, ri = iread, n, i;
    int aux;
//    if(size != 2 || data[0] > nr ||  data[1] >= nr) {
    if(data[0] > nr ||  data[1] >= nr) {
        send_confirmation(TRGI, CMD_ERROR);
        return;
    }
    n = data[0];
    i = data[1];
    aux = i;

    i = (i + wi) % nr;
    putch(SOM);
    putch(TRGI);
    if(!transfer_registers(n, i, aux))
        putch(CMD_ERROR);
    else putch(EOM);

 }

bool transfer_registers(uint8_t n, uint8_t i, int aux) {
    uint8_t N = read_nreg(), wi = read_iwrt(), nr = ring_buffer_laped() ? N : wi;
    int c = 0, i_ = i;
    uint8_t not = aux == -1 ? get_not_transferred() : nr - aux;
    for(c = 0; c < n && c < not; i_ = (i_ + 1) % N, ++c);
    putch(c);
    if(aux != -1) putch((uint8_t) aux);
    for(int c = 0; c < n && c < not; i = (i + 1) % nr, ++c){
        send_register(i);
        if(i == iread) { 
            iread = (iread + 1) % N;
            dec_not_transferred();
        }
    }    
    return true;
}

void notification_memory(void) {
    uint8_t N = read_nreg(), wi = read_iwrt(), nr = ring_buffer_laped() ? N : wi, ri = iread;
    char msg[] = {NMFL, N, nr, ri, wi};
    send_msg(msg, 5);
}

void send_confirmation(uint8_t cmd, uint8_t error) {
    char msg[] = {cmd, error};
    send_msg(msg, 2);
}

void send_msg(char * msg, uint8_t size) {
    putch(SOM);
    for(int i = 0; i < size; ++i) putch(msg[i]);
    putch(EOM);
}

void send_register(uint8_t i) {
    for(int k = 0; k < 5; ++k)
        putch(read_buffer(i, k));
}

void countMsg(void) {
    EUSART_Receive_ISR();
    if(EUSART_LastByte() == EOM) {
        msgs++;
    }
        
}

void inc_not_transferred(void){ if(not_transferred < read_nreg()) not_transferred++; }
void dec_not_transferred(void){ if(not_transferred > 0) not_transferred--; }
void set_overlaped(void) { overlaped = true; }
void reset_overlaped(void) { overlaped = false; }
uint8_t get_not_transferred(void) { return not_transferred; }
void set_iread(uint8_t i) { iread = i; }