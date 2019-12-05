#include "structure.h"

//Sycronization stuff
extern cyg_handle_t user_com_channel_H;
extern cyg_handle_t com_user_channel_H;
extern void init_req(request*);
extern void init_ack(acknowledge*);
extern void init_reg(buffer*);
extern void add_pmem(int, int, int, int, int);
extern void copy_reg(buffer*, buffer*);
extern void list_pmem(void);
extern pic_memory pmem;

typedef struct Placa{
    int clock_second;
    int clock_minute;
    int clock_hour;
    int temperature;
    int luminosity;
    int pmon;
    int tala;
    int temperature_alarm;
    int luminosity_alarm;
    int is_active_alarm;
} placa;

static placa p;
static request *req = NULL;
static request *req_user = NULL;
static acknowledge *ack = NULL;
static acknowledge *ack_user = NULL;
static buffer *reg = NULL;

// void pic(void);

int getClockHour(void){return p.clock_hour;}
int getClockMinute(void){return p.clock_minute;}
int getClockSecond(void){return p.clock_second;}
int getTemperature(void){return p.temperature;}
int getLuminosity(void){return p.luminosity;}
int getPMON(void){return p.pmon;}
int getTALA(void){return p.tala;}
int getTemperatureAlarm(void){return p.temperature_alarm;}
int getLuminosityAlarm(void){return p.luminosity_alarm;}
int getIsActiveAlarm(void){return p.is_active_alarm;}
int getNREG(void){return NREG;}
int getNr(void){return pmem.nr;}
int getIread(void){return pmem.iread;}
int getIwrite(void){return pmem.iwrite;}

bool setClockHour(int h){p.clock_hour=h;return true;}
bool setClockMinute(int m){p.clock_minute=m;return true;}
bool setClockSecond(int s){p.clock_second=s;return true;}
bool setPMON(int mp){p.pmon=mp;return true;}
bool setTALA(int a){p.tala=a;return true;}
bool setTemperatureAlarm(int ta){p.temperature_alarm=ta; return true;}
bool setLuminosityAlarm(int la){p.luminosity_alarm=la; return true;}
bool setIsActiveAlarm(int iaa){p.is_active_alarm=iaa; return true;}

void init_placa(void){
    p.clock_second=10;
    p.clock_minute=15;
    p.clock_hour=20;
    p.temperature=25;
    p.luminosity=5;
    p.pmon=10;
    p.tala=20;
    p.temperature_alarm=30;
    p.luminosity_alarm=10;
    p.is_active_alarm=0;

    init_ack(ack);
    init_req(req);
    init_reg(reg); //TODO: remover isto
    add_pmem(20, 3, 0, 5, 1);
    add_pmem(23, 4, 0, 15, 2);
    add_pmem(23, 3, 0, 25, 3);
    add_pmem(25, 2, 0, 35, 4);
    add_pmem(26, 2, 0, 45, 5);
    add_pmem(25, 1, 0, 55, 6);
    add_pmem(28, 2, 1, 5, 7);
    add_pmem(25, 4, 1, 25, 8);
    add_pmem(24, 3, 1, 45, 9);
    add_pmem(23, 2, 1, 55, 10);
    add_pmem(22, 3, 2, 35, 11);
    add_pmem(21, 3, 3, 25, 12);
    add_pmem(20, 2, 4, 25, 13);
    add_pmem(18, 2, 18, 35, 14);
    add_pmem(17, 3, 22, 45, 15);
}

void pic(void){
    bool exit = false;
    int i;
    //int c;
    req = (request *)malloc(sizeof(request));
    ack = (acknowledge *)malloc(sizeof(acknowledge));
    reg = (buffer *)malloc(sizeof(buffer));
    init_placa();
    //list_pmem();
    while(!exit){
        req_user = (request *) cyg_mbox_get(user_com_channel_H);
        switch (req_user->cmd){
        case CODE_RC:
            init_req(req);
            req->arg[0]=getClockHour();
            req->arg[1]=getClockMinute();
            req->arg[2]=getClockSecond();
            cyg_mbox_put(com_user_channel_H, req);
            break;
        case CODE_SC:
            if (setClockHour(req_user->arg[0]) && setClockMinute(req_user->arg[1]) && setClockSecond(req_user->arg[2]))
                ack->error=false;
            else 
                ack->error=true;
            cyg_mbox_put(com_user_channel_H, ack);
            break;
        case CODE_RTL:
            init_req(req);
            req->arg[0]=getTemperature();
            req->arg[1]=getLuminosity();
            cyg_mbox_put(com_user_channel_H, req);
            break;
        case CODE_RP:
            init_req(req);
            req->arg[0]=getPMON();
            req->arg[1]=getTALA();
            cyg_mbox_put(com_user_channel_H, req);
            break;
        case CODE_MMP:
            if (setPMON(req_user->arg[0])) ack->error=false;
            else  ack->error=true;
            cyg_mbox_put(com_user_channel_H, ack);
            break;
        case CODE_MTA:
            if (setTALA(req_user->arg[0])) ack->error=false;
            else ack->error=true;
            cyg_mbox_put(com_user_channel_H, ack);
            break;
        case CODE_RA:
            init_req(req);
            req->arg[0]=getTemperatureAlarm();
            req->arg[1]=getLuminosityAlarm();
            req->arg[2]=getIsActiveAlarm();
            cyg_mbox_put(com_user_channel_H, req);
            break;
        case CODE_DTL:
            if (setTemperatureAlarm(req->arg[0]) && setLuminosityAlarm(req->arg[1])) ack->error=false;
            else ack->error=true;
            cyg_mbox_put(com_user_channel_H, ack);
            break;
        case CODE_AA:
            if (setIsActiveAlarm(req->arg[0])) ack->error=false;
            else ack->error=true;
            cyg_mbox_put(com_user_channel_H, ack);
            break;
        case CODE_IR:
            init_req(req);
            req->arg[0]=getNREG();
            req->arg[1]=getNr();
            req->arg[2]=getIread();
            req->arg[3]=getIwrite();
            cyg_mbox_put(com_user_channel_H, ack);
            break;
        case CODE_TRC:
            //TODO: tudo isto deve ser substituido 
            // n = req->arg[0]
            i=0;
            ack->error=true;
            for(i=pmem.iread; i<pmem.iwrite && i-pmem.iread<req_user->arg[0]; i++){
                copy_reg(reg, &(pmem.reg[i]));
                cyg_mbox_put(com_user_channel_H, reg); //Envia registo 
                ack_user = cyg_mbox_get(user_com_channel_H); //espera pelo ok do user
            }
            pmem.iread=i; //atualiza o valor de iread TODO:verificar este valor
            reg->hour=-1;
            cyg_mbox_put(com_user_channel_H, reg); //Envia um poisoned reg para assinalar que ja acabou
            break;
        case CODE_TRI:
            //TODO: tudo isto deve ser substituido 
            // n = req->arg[0]
            // i = req->arg[1]
            i=0;
            //c=0;
            ack->error=true;
            //printf("initial=%d, i<%d and i<%d\n", req_user->arg[1], pmem.iwrite, req_user->arg[0]+req_user->arg[1]);
            for(i=req_user->arg[1]; i<pmem.iwrite && i<(req_user->arg[0]+req_user->arg[1]); i++){
                //printf("for: %d, i=%d\n", c, i);
                copy_reg(reg, &(pmem.reg[i]));
                //printf("%d:%d:%d - %d C - %d lm\n", reg->hour, reg->minute, reg->second, reg->temperature, reg->luminosity);
                cyg_mbox_put(com_user_channel_H, reg); //Envia o registo
                ack_user = cyg_mbox_get(user_com_channel_H); //obter o ack do user (forma de bloquear a thread)
                //c++;
            }
            reg->hour=-1;
            cyg_mbox_put(com_user_channel_H, reg); //Envia um reg com o valor de horas negativo para sinalizar que já acabou
            break;
        case CODE_EXIT:
            exit=true;
            break;
        default:
            break;
        }
    }
    free(ack);
    free(req);
    free(reg);
    cyg_thread_exit();
}


///////////////////////////////////////////////////////////////////
///////////////////////////SEND TO PIC/////////////////////////////
///////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////
///////////////////////////READ FROM PIC///////////////////////////
///////////////////////////////////////////////////////////////////


extern cyg_io_handle_t serH;
static cyg_uint32 len = 1;
static uint8_t cmd;
bool toSend;
static request *reply;
//registers transference
static uint8_t *regs;
static uint8_t n;
static uint8_t i;


void send_error(){
    acknowledge * a = (acknowledge *) malloc (sizeof(acknowledge));
    a.error = false;
    cyg_mbox_put(com_user_channel_H, a);
}

void send_ack(){
    acknowledge * a = (acknowledge *) malloc (sizeof(acknowledge));
    a.error = false;
    cyg_mbox_put(com_user_channel_H, a);
}

bool read_command(int size){
    reply = (request *) malloc(size * sizeof(request));
    int i = 0;

    do{
        cyg_io_read(serH, &reply.arg[i], &len);
    } while (reply.arg[i++] != CMD_ERROR && i < size);

    if (reply.arg[(i-1] == EOM){
        reply.cmd = cmd;
        return true;
    }
    
    send_error();
    free (reply);
    return false;
}

void read_regs(bool indexed){
    regs = (uint8_t *) malloc((5*n + 1) * sizeof(uint8_t));
    int j = 5;
    for (int i = 0; i < n && j > 0; ++i)
        for(j = 0; j < 5; ++j)
            cyg_io_read(&regs[i+j], &len);
    
    cyg_io_read(&regs[i+j], &len);
    if (regs[i+j] != EOM){
        send_error();
        return;
    }

    for (int i = 0; i < 5*n; i+=5)
        add_local(regs[i], regs[i+1], regs[i+2], regs[i+3], regs[i+4]);

    free(regs);
    send_ack();
}

void read_pic(void){
    uint8_t start;
    while (1){
        do {
            cyg_io_read(serH, &start, &len);
        } while (start != SOM);

        cyg_io_read(serH, &cmd, &len);         

        switch(cmd){
            case RCLK:
                toSend = read_command(6);
                break;
            case SCLK:cyg_mbox_put(com_user_channel_H, &reply);
                if (read_command(4))
                    send_ack();
                toSend = false;
                break;
            case RTL:
                toSend = read_command(5);
                break;
            case RPAR:
                toSend = read_command(5);
                break;
            case MMP:
                if (read_command(4))
                    send_ack();
                toSend = false;
                break;
            case MTA:
                if (read_command(4))
                    send_ack();
                toSend = false;
                break;
            case RALA:
                toSend = read_command(6);
                break;
            case DATL:
                if (read_command(4))
                    send_ack();
                toSend = false;
                break;
            case AALA:
                if (read_command(4))
                    send_ack();
                toSend = false;
                break;
            case IREG:
                toSend = read_command(7);
                break;
            case TRGC:
                cyg_io_read(&n, &len);
                if (n == CMD_ERROR || n == EOM)
                    send_error();
                else 
                    read_regs(false);
                toSend = false;
                break;
            case TRGI:
                cyg_io_read(&n, &len);
                if (n == CMD_ERROR || n == EOM)
                    send_error();
                else 
                    cyg_io_read(&i, &len);
                if (i == CMD_ERROR ||i == EOM)
                    send_error();
                else
                    read_regs(true);
                toSend = false;
                break;
            case NMFL:
                if (read_command(7)){
                    cyg_mutex_lock(&stdin_mutex);
                    printf("\nNMFL received:\n N = %d, nr = %d, ri = %d, wi = %d\n\nCmd>", 
                            reply.arg[1], reply.arg[2], reply.arg[3], reply.arg[4]);
                    cyg_mutex_unlock(&stdin_mutex);
                    
                    if ( (iw - ir) == N ){
                        cyg_mutex_lock(&stdin_mutex);
                        printf("\nwarning: memory full!\n\nCmd>");
                        cyg_mutex_unlock(&stdin_mutex);
                    }

                    if (! IsAlarmActive()){
                        setAlarmPeriod(1);
                        activateAlarm();
                    }
                }
                free(reply);
                toSend = false;
                break;
            case default:
                send_error();
                toSend = false;
                break;
        }

        if (toSend)
            cyg_mbox_put(com_user_channel_H, &reply);
    }
}