#include "structure.h"

//Sycronization stuff
extern cyg_handle_t user_com_channel_H;
extern cyg_handle_t com_user_channel_H;
extern void init_req(request*);
extern void init_ack(acknowledge*);
extern void init_reg(buffer*);
extern void add_pmem(cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8);
extern void copy_reg(buffer*, buffer*);
extern void list_pmem(void);
/*/extern pic_memory pmem;

typedef struct Placa{
    cyg_uint8 clock_second;
    cyg_uint8 clock_minute;
    cyg_uint8 clock_hour;
    cyg_uint8 temperature;
    cyg_uint8 luminosity;
    cyg_uint8 pmon;
    cyg_uint8 tala;
    cyg_uint8 temperature_alarm;
    cyg_uint8 luminosity_alarm;
    cyg_uint8 is_active_alarm;
} placa;

static placa p;
static request *req = NULL;*/
static request *req_user = NULL;
/*static acknowledge *ack = NULL;
static acknowledge *ack_user = NULL;
static buffer *reg = NULL;

// void pic(void);

cyg_uint8 getClockHour(void){return p.clock_hour;}
cyg_uint8 getClockMinute(void){return p.clock_minute;}
cyg_uint8 getClockSecond(void){return p.clock_second;}
cyg_uint8 getTemperature(void){return p.temperature;}
cyg_uint8 getLuminosity(void){return p.luminosity;}
cyg_uint8 getPMON(void){return p.pmon;}
cyg_uint8 getTALA(void){return p.tala;}
cyg_uint8 getTemperatureAlarm(void){return p.temperature_alarm;}
cyg_uint8 getLuminosityAlarm(void){return p.luminosity_alarm;}
cyg_uint8 getIsActiveAlarm(void){return p.is_active_alarm;}
cyg_uint8 getNREG(void){return NREG;}
cyg_uint8 getNr(void){return pmem.nr;}
cyg_uint8 getIread(void){return pmem.iread;}
cyg_uint8 getIwrite(void){return pmem.iwrite;}

bool setClockHour(cyg_uint8 h){p.clock_hour=h;return true;}
bool setClockMinute(cyg_uint8 m){p.clock_minute=m;return true;}
bool setClockSecond(cyg_uint8 s){p.clock_second=s;return true;}
bool setPMON(cyg_uint8 mp){p.pmon=mp;return true;}
bool setTALA(cyg_uint8 a){p.tala=a;return true;}
bool setTemperatureAlarm(cyg_uint8 ta){p.temperature_alarm=ta; return true;}
bool setLuminosityAlarm(cyg_uint8 la){p.luminosity_alarm=la; return true;}
bool setIsActiveAlarm(cyg_uint8 iaa){p.is_active_alarm=iaa; return true;}

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
    cyg_uint8 i;
    //cyg_uint8 c;
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
*/

///////////////////////////////////////////////////////////////////
////////////////////////////SEND TO PIC////////////////////////////
///////////////////////////////////////////////////////////////////

extern cyg_io_handle_t serH;

cyg_uint8 message1 [1 + 1 + 1] = {SOM, 0, EOM}; // SOM + msg + EOM
cyg_uint8 message2 [1 + 2 + 1] = {SOM, 0, 0, EOM};
cyg_uint8 message3 [1 + 3 + 1] = {SOM, 0, 0, 0, EOM};
cyg_uint8 message4 [1 + 4 + 1] = {SOM, 0, 0, 0, 0, EOM};

void send_msg(cyg_uint8 cmd, unsigned int size){
    cyg_uint8 * msg = NULL;
    if      (size == 1) msg = message1;
    else if (size == 2) msg = message2;
    else if (size == 3) msg = message3;
    else if (size == 4) msg = message4;


    msg[1] = cmd;
    cyg_uint8 pos;
    for (pos = 2; pos <= size; ++pos)
        msg[pos] = req_user->arg[pos-2];

  size += 2;
printf("lalala %x\n", msg[1]);
    cyg_io_write(serH, msg, &size);
//printf("SENT\n");
}

void write_pic(void){
//printf("Write PIC\n");
    while(1){
//printf("Before user_com_channel\n");
        req_user = (request *) cyg_mbox_get(user_com_channel_H);
//printf("After user_com_channel HEY: %d\n", req_user->cmd);
        switch (req_user->cmd){
        case CODE_RC:
//printf("SENDING\n");
            send_msg(RCLK, 1);
            break;
        case CODE_SC:
            send_msg(SCLK, 4);
            break;
        case CODE_RTL:
            send_msg( RTL, 1);
            break;
        case CODE_RP:
            send_msg(RPAR, 1);
            break;
        case CODE_MMP:
            send_msg( MMP, 2);
            break;
        case CODE_MTA:
            send_msg( MTA, 2);
            break;
        case CODE_RA:
            send_msg(RALA, 1);
            break;
        case CODE_DTL:
            send_msg(DATL, 3);
            break;
        case CODE_AA:
            send_msg(AALA, 2);
            break;
        case CODE_IR:
            send_msg(IREG, 1);
            break;
        case CODE_TRC:
            send_msg(TRGC, 2);
            break;
        case CODE_TRI:
            send_msg(TRGI, 3);
            break;
        default:
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////
///////////////////////////READ FROM PIC///////////////////////////
///////////////////////////////////////////////////////////////////

extern void activateAlarm(void);
extern bool IsAlarmActive(void);
extern void setAlarmPeriod(cyg_uint8);

extern cyg_mutex_t stdin_mutex;
extern void add_local( cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8);
cyg_uint32 len = 1;
cyg_uint8 cmd;
bool toSend;
request *reply;
//registers transference
//cyg_uint8 *regs;
cyg_uint8 n;
cyg_uint8 i;


void send_error(void){
    acknowledge * a = (acknowledge *) malloc (sizeof(acknowledge));
    a->error = true;
    cyg_mbox_put(com_user_channel_H, a);
}

void send_ack(void){
    acknowledge * a = (acknowledge *) malloc (sizeof(acknowledge));
    a->error = false;
    cyg_mbox_put(com_user_channel_H, a);
}

bool read_command(cyg_uint8 size){
    reply = (request *) malloc(size * sizeof(request));
    cyg_uint8 i_ = 0;

//printf("read_command\n");
    do{
        cyg_io_read(serH, &reply->arg[i_], &len);
	//printf("read: %d\n", reply->arg[i_]);
    } while (reply->arg[i_++] != CMD_ERROR && i_ < size && reply->arg[i_-1] != EOM);
int x; 
	for(x = 0; x < i_; x++) //printf("%do é %u\n", x, reply->arg[x]);
    if (reply->arg[i_-1] == EOM){
	//printf("HERE\n");
        reply->cmd = cmd;
        return true;
    }
    
    send_error();
    free (reply);
    return false;
}

void read_regs(bool indexed){
    cyg_uint8 * regs = (cyg_uint8 *) malloc((5*n + 1) * sizeof(cyg_uint8));
    cyg_uint8 i_ = 0;
    cyg_uint8 j = 5;

printf("Before xxx\n");
    for (i_ = 0; i_ < n && j > 0; ++i_)
        for(j = 0; j < 5; ++j)
            cyg_io_read(serH, &regs[i_*5+j], &len);
    printf("After xxx\n");
int x;
	for(x = 0; x < n * 5; x++) printf("FOR %u\n", regs[x]);
	
    cyg_io_read(serH, &regs[i_*5+j], &len);
    if (regs[i_*5+j] != EOM){
        send_error();
        return;
    }

    for (i_ = 0; i_ < 5*n; i_+=5)
{
	
	//printf("REG: %d - %d - %d - %d - %d\n", regs[i_], regs[i_+1], regs[i_+2], regs[i_+3], regs[i_+4]);
        add_local(regs[i_], regs[i_+1], regs[i_+2], regs[i_+3], regs[i_+4]);
}
    free(regs);
    send_ack();
}

void read_pic(void){
    cyg_uint8 start;
//printf("ENTERING\n");
    while (1){
//printf("read_pic while\n");
        do {
//printf("before read_pic cyg_io_read\n");
            cyg_io_read(serH, &start, &len);
printf("after read_pic cyg_io_read %x\n", start);
        } while (start != SOM);

        cyg_io_read(serH, &cmd, &len);         
printf("%x\n", cmd);

        switch(cmd){
            case RCLK:
                toSend = read_command(6);
		//printf("toSend: %d\n", toSend);
                break;
            case SCLK:
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
                cyg_io_read(serH, &n, &len);
                if (n == CMD_ERROR || n == EOM){
			//printf("IF\n");
                    send_error();
			//printf("IF OUT\n");
		}
                else {
			//printf("ELSE\n");
                    read_regs(false);
			//printf("ELSE OUT\n");
		}
                toSend = false;
                break;
            case TRGI:
printf("AQUI\n");
                cyg_io_read(serH, &n, &len);
                if (n == CMD_ERROR || n == EOM)
                    send_error();
                else 
                    cyg_io_read(serH, &i, &len);
                if (i == CMD_ERROR ||i == EOM)
		{
	printf("Error\n");
                    send_error();
			break;
		}
                else
                    read_regs(true);
                toSend = false;
                break;
            case NMFL:
                if (read_command(7)){
                    cyg_mutex_lock(&stdin_mutex);
                    printf("\nNMFL received:\n N = %d, nr = %d, ri = %d, wi = %d\n\nCmd>", 
                            reply->arg[0], reply->arg[1], reply->arg[2], reply->arg[3]);
                    cyg_mutex_unlock(&stdin_mutex);
                    
                    if ( (reply->arg[3] - reply->arg[2]) == reply->arg[0] ){
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
            default:
                send_error();
                toSend = false;
                break;
        }

        if (toSend){
	toSend = false;
	//printf("toSend\n");
            cyg_mbox_put(com_user_channel_H, reply);
	//printf("toSend sent\n");
	}
    }
}
