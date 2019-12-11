#include "structure.h"

//Sycronization variables
extern cyg_handle_t user_com_channel_H;
extern cyg_handle_t com_user_channel_H;
extern void init_req(request*);
extern void init_ack(acknowledge*);
extern void init_reg(buffer*);
extern void add_pmem(cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8);
extern void copy_reg(buffer*, buffer*);
extern void list_pmem(void);

static request *req_user = NULL;

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
    cyg_io_write(serH, msg, &size);
}

void write_pic(void){
    while(1){
        req_user = (request *) cyg_mbox_get(user_com_channel_H);
        switch (req_user->cmd){
        case CODE_RC:
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
        free(req_user);
    }
}

///////////////////////////////////////////////////////////////////
///////////////////////////READ FROM PIC///////////////////////////
///////////////////////////////////////////////////////////////////

extern void activateAlarm(void);
extern bool IsAlarmActive(void);
extern void setAlarmPeriod(cyg_uint8);

//Syncronization variables
extern cyg_mutex_t stdin_mutex;

extern void add_local( cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8);

cyg_uint32 len = 1;
cyg_uint8 cmd;
bool toSend;
request *reply;

//registers transference
cyg_uint8 n;
cyg_uint8 i;

//event flag
extern cyg_flag_t ef;

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
    do{
        cyg_io_read(serH, &reply->arg[i_], &len);
    } while (reply->arg[i_++] != CMD_ERROR && i_ < size && reply->arg[i_-1] != EOM);

    if (reply->arg[i_-1] == EOM){
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

    for (i_ = 0; i_ < n && j > 0; ++i_)
        for(j = 0; j < 5; ++j)
            cyg_io_read(serH, &regs[i_*5+j], &len);
    cyg_io_read(serH, &regs[i_*5+j], &len);
    if (regs[i_*5+j] != EOM){
        send_error();
	free(regs);
        return;
    }

    for (i_ = 0; i_ < 5*n; i_+=5)
        add_local(regs[i_], regs[i_+1], regs[i_+2], regs[i_+3], regs[i_+4]);
    free(regs);
    send_ack();
    //awake event flag
    cyg_flag_value_t efv=0x01;
    cyg_flag_setbits(&ef, efv);
}

void read_pic(void){
    cyg_uint8 start;
    while (1){
        do {
            cyg_io_read(serH, &start, &len);
        } while (start != SOM);

        cyg_io_read(serH, &cmd, &len);         
        switch(cmd){
            case RCLK:
                toSend = read_command(6);
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
                if (n == CMD_ERROR || n == EOM)
                    send_error();
                else 
                    read_regs(false);
                toSend = false;
                break;
            case TRGI:
                cyg_io_read(serH, &n, &len);
                if (n == CMD_ERROR || n == EOM)
                    send_error();
                else 
                    cyg_io_read(serH, &i, &len);
                if (i == CMD_ERROR ||i == EOM){
                    send_error();
                    break;
                } else
                    read_regs(true);
                toSend = false;
                break;
            case NMFL:
                if (read_command(7)){
                    cyg_mutex_lock(&stdin_mutex);
                    printf("\nNMFL received:\n N = %d, nr = %d, ri = %d, wi = %d\n\nCmd> ", 
                            reply->arg[0], reply->arg[1], reply->arg[2], reply->arg[3]);
                    cyg_mutex_unlock(&stdin_mutex);
                    if ( (reply->arg[3] - reply->arg[2]) == reply->arg[0] ){
                        cyg_mutex_lock(&stdin_mutex);
                        printf("\nwarning: memory full!\n\nCmd> ");
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
        if (toSend){
		toSend = false;
		cyg_mbox_put(com_user_channel_H, reply);
	}
	
    }
}
