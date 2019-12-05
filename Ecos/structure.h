#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <cyg/kernel/kapi.h>

#define REQUEST_SIZE 10
#define CMD_LEN 5
#define N_ARGS 10
#define NRBUF 100
#define NREG 20 //TODO: For debugging only

//Request codes
#define CODE_EXIT -1
#define CODE_RC 1
#define CODE_SC 2
#define CODE_RTL 3
#define CODE_RP 4
#define CODE_MMP 5
#define CODE_MTA 6
#define CODE_RA 7
#define CODE_DTL 8
#define CODE_AA 9
#define CODE_IR 10
#define CODE_TRC 11
#define CODE_TRI 12
#define CODE_IRL 13
#define CODE_LR 14
#define CODE_DR 15
#define CODE_CPT 16
#define CODE_MPT 17
#define CODE_CTTL 18
#define CODE_DTTL 19
#define CODE_PR 20

/* It is assumed that SOM and EOM values do not occur in the message */
#define SOM 0xFD /* start of message */
#define EOM 0xFE /* end of message */
#define RCLK 0xC0 /* read clock */
#define SCLK 0XC1 /* set clock */
#define RTL 0XC2 /* read temperature and luminosity */
#define RPAR 0XC3 /* read parameters */
#define MMP 0XC4 /* modify monitoring period */
#define MTA 0XC5 /* modify time alarm */
#define RALA 0XC6 /* read alarms (temperature, luminosity, active/inactive) */
#define DATL 0XC7 /* define alarm temperature and luminosity */
#define AALA 0XC8 /* activate/deactivate alarms */
#define IREG 0XC9 /* information about registers (NREG, nr, iread, iwrite)*/
#define TRGC 0XCA /* transfer registers (curr. position)*/
#define TRGI 0XCB /* transfer registers (index) */
#define NMFL 0XCC /* notification memory (half) full */
#define CMD_OK 0 /* command successful */
#define CMD_ERROR 0xFF /* error in command */

//Request message struct
typedef struct Request {
    int cmd;
    int arg[N_ARGS];
} request;

//Acknowledge message struct
typedef struct Acknowledge {
    bool error;
} acknowledge;

//Register message struct
typedef struct Buffer {
    int temperature;
    int luminosity;
    int hour;
    int minute;
    int second;
} buffer ;

//Local memory struct
typedef struct Local {
    int nr;
    int iread;
    int iwrite; //head
    buffer reg[NRBUF];
} local;

//Pic memory struct
typedef struct PicMemory{
    int nr;
    int iread;
    int iwrite;
    buffer reg[NREG];
} pic_memory;


///////////ALARRRMMM DOWN HERE/////////////

typedef struct alarmStuff_{
    cyg_mutex_t mutex;
    bool issued;
    int period;
    cyg_handle_t id;
} alarmStuff;

alarmStuff alarm;

//alarmHandler
void alarmfn(cyg_handle_t alarmH, cyg_addrword_t data){
    alarm.issued = true;
}

void alarm_init(){
    cyg_mutex_init(&alarm.mutex);
    alarm.issued = false;
    alarm.period = 0;

    cyg_handle_t alarmCounter;
    cyg_alarm alarm_;
    cyg_clock_to_counter(cyg_real_time_clock(), &alarmCounter);
    cyg_alarm_create(alarmCounter, alarmfn,
    (cyg_addrword_t) 0, &alarm.id, &alarm_);
}

void activateAlarm(){
    int period;
    cyg_handle_t id;

    if (period == 0) return;

    cyg_mutex_lock(&alarm.mutex);
    alarm.active = true;
    period = alarm.period;
    id = alarm.id;
    cyg_mutex_unlock(&alarm.mutex);

    cyg_alarm_initialize(id, cyg_current_time(), period*100*60);
}

void deactivateAlarm(){
    cyg_handle_t id;

    cyg_mutex_lock(&alarm.mutex);
    id = alarm.id;
    alarm.period = 0;
    cyg_mutex_unlock(&alarm.mutex);

    cyg_alarm_disable(id);
}

bool IsAlarmActive(){
    bool ret;

    cyg_mutex_lock(&alarm.mutex);
    ret = alarm.period > 0;
    cyg_mutex_unlock(&alarm.mutex);

    return ret;
}

bool IsAlarmIssued(){
    bool ret;

    cyg_scheduler_lock();
    ret = alarm.issued;
    alarm.issued = 0; //se n for só usado no processing p fazer a transferencia, tem q se tirar isto daqui
    cyg_scheduler_unlock();

    return ret;
}

int getAlarmPeriod(){
    bool ret;

    cyg_mutex_lock(&alarm.mutex);
    ret = alarm.period;
    cyg_mutex_unlock(&alarm.mutex);

    return ret;
}

void setAlarmPeriod(int period){
    cyg_mutex_lock(&alarm.mutex);
    alarm.active = true;
    period = alarm.period;
    id = alarm.id;
    cyg_mutex_unlock(&alarm.mutex);
}

#endif