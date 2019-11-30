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

void read_msgs() {
    char aux[8];
    uint8_t cmd = 0;
    int i = 0;

    do {
        cmd = EUSART_Read();
       // putch(cmd);
    } while (cmd != SOM && EUSART_is_rx_ready());

    for(i = 0; cmd != EOM && EUSART_is_rx_ready(); i++) {
        cmd = EUSART_Read();
        aux[i] = cmd;
    }
    aux[i] = '\0';

    interpret_message(aux);
}

void interpret_message(char * msg) {
    uint8_t cmd = msg[0];
    char * data = strlen(msg) != 1 ? msg + 1 : NULL;
    
    switch(cmd) {
        case RCLK:
            read_clock();
            break;
        case SCLK:
            set_clock(data);
            break;
        case RTL:
            read_temp_lum();
            break;
        case RPAR:
            read_param();
            break;
        case MMP:
            modify_pmon(data);
            break;
        case MTA:
            modify_time_alarm(data);
            break;
        case RALA:
            read_alarms();
            break;
        case DATL:
            define_alarm(data);
            break;
        case AALA:
            switch_alarm(data);
            break;
        case IREG:
            registers_info();
            break;
        case TRGC:
            transfer_n_registers(data);
            break;
        case TRGI:
            transfer_registers_i(data);
            break;
        default:
            send_confirmation(cmd, CMD_ERROR);
            break;
    }
}

void read_clock() {
    INTERRUPT_PeripheralInterruptDisable();
    uint8_t hours = clkh, minutes = clkm, seconds = clks; // avoid conflicts in interrupt
    INTERRUPT_PeripheralInterruptEnable();
    char msg[] = {RCLK, hours, minutes, seconds, '\0'};
    send_msg(msg);
}

void set_clock(char * data) {
    uint8_t hours, minutes, seconds;
    if(!data || strlen(data) != 3 || (hours = data[0] > 23) || (minutes = data[1] > 59) || (seconds = data[2] > 59)) {
        send_confirmation(SCLK, CMD_ERROR);
        return;
    }
    INTERRUPT_PeripheralInterruptDisable();
    clkh = hours; clkm = minutes; clks = seconds; // avoid conflicts in interrupt
    INTERRUPT_PeripheralInterruptEnable();
    update_clk();
    send_confirmation(SCLK, CMD_OK);
}

void read_temp_lum() {
    char msg[] = {RTL, temp, illum, '\0'};
    send_msg(msg);
}

void read_param() {
    char msg[] = {RPAR, pmon, tala, '\0'};
    send_msg(msg);
}

void modify_pmon(char * data) {
    if(!data || strlen(data) != 1 || data[0] > 99) {
        send_confirmation(MMP, CMD_ERROR);
        return;
    }
    pmon = data[0];
    write_pmon(pmon);
    send_confirmation(MMP, CMD_OK);
}

void modify_time_alarm(char * data) {
    if(!data || strlen(data) != 1 || data[0] > 60) {
        send_confirmation(MTA, CMD_ERROR);
        return;
    }
    tala = data[0];
    write_tala(tala);
    send_confirmation(MTA, CMD_OK);
}

void read_alarms() {
    char msg[] = {RALA, alat, alal, alaf, '\0'};
    send_msg(msg);
}

void define_alarm(char * data) {
    if(!data || strlen(data) != 2 || data[0] > 50 || data[1] > 3) {
        send_confirmation(DATL, CMD_ERROR);
        return;
    }
    alat = data[0]; alal = data[1];
    send_confirmation(DATL, CMD_OK);
}

void switch_alarm(char * data) {
    if(!data || strlen(data) != 1 || data[0] > 1) {
        send_confirmation(AALA, CMD_ERROR);
        return;
    }
    alaf = data[0];
    write_alaf(alaf);
    send_confirmation(AALA, CMD_OK);
}

void registers_info() {
    uint8_t N = read_nreg(), wi = read_iwrt(), nr = ring_buffer_laped() ? N : wi, ri = iread;
    // TODO: change ri & ask about wi
    char msg[] = {IREG, N, nr, ri, wi, '\0'};
    send_msg(msg);
}

void transfer_n_registers(char * data) {
    uint8_t nr = ring_buffer_laped() ? read_nreg() : read_iwrt(), ri = iread, n;
    if(!data || strlen(data) != 1 || (n = data[0]) > nr) {
        send_confirmation(TRGC, CMD_ERROR);
        return;
    }
    if(!transfer_registers(n, iread))
        send_confirmation(TRGC, CMD_ERROR);
    else putch(EOM);;
}

void transfer_registers_i(char * data) {
    uint8_t N = read_nreg(), wi = read_iwrt(), nr = ring_buffer_laped() ? N : wi, ri = iread, n, i;
    bool sanity = true;
    // TODO: same
    if(!data || strlen(data) != 2 || (n = data[0]) > nr || (i = data[1]) > nr - 1) {
        send_confirmation(TRGI, CMD_ERROR);
        return;
    }

    i = (i + wi) % nr; // certo

    if(!transfer_registers(n, i))
        send_confirmation(TRGI, CMD_ERROR);
    else putch(EOM);

 }

bool transfer_registers(uint8_t n, uint8_t i) {
    uint8_t N = read_nreg(), wi = read_iwrt(), nr = ring_buffer_laped() ? N : wi;

    if(i < wi) {
        if(i + n - 1 >= wi) return false;
    } else if(i + n > N && i + n - 1 - N > wi) return false;

    for(int c = 0; c < n; i = (i + 1) % nr, ++c){
        send_register(i);
        if(i == iread) iread = (iread + 1) % N;
    }    
    return true;
}

void notification_memory() {
    uint8_t N = read_nreg(), wi = read_iwrt(), nr = ring_buffer_laped() ? N : wi, ri = iread;
    char msg[] = {NMFL, N, nr, ri, wi, '\0'};
    send_msg(msg);
}

void send_confirmation(uint8_t cmd, uint8_t error) {
    char msg[] = {cmd, error, '\0'};
    send_msg(msg);
}

void send_msg(char * msg) {
    EUSART_Write(SOM);
    for(int i = 0; i < strlen(msg); ++i) EUSART_Write(msg[i]);
    //printf(msg);
    EUSART_Write(EOM);
}

void send_register(uint8_t i) {
    for(int k = 0; k < 5; ++k)
        putch(read_buffer(i, k));
}

void countMsg() {
    EUSART_Receive_ISR();
    if(EUSART_LastByte() == EOM) {
        msgs++;
    }
}