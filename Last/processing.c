#include "structure.h"

//Sycronization stuff
extern cyg_handle_t user_pro_channel_H;
extern cyg_handle_t pro_user_channel_H;
extern void init_req(request *);
extern void init_ack(acknowledge*);
extern void process_local(cyg_uint8 [], request *);
extern void process(void);
extern void init_process(void);

extern void setTemperatureThreshold(cyg_uint8);
extern void setLuminosityThreshold(cyg_uint8);
extern cyg_uint8 getTemperatureThreshold(void);
extern cyg_uint8 getLuminosityThreshold(void);
extern cyg_uint8 getAlarmPeriod(void);
extern void activateAlarm(void);
extern void setAlarmPeriod(cyg_uint8);

extern cyg_flag_t ef;
extern cyg_handle_t user_com_channel_H;
extern cyg_handle_t com_user_channel_H;

static request * req_user = NULL;
static request * req = NULL;
static acknowledge * ack = NULL;

void processing(void);

void processing(void){
    bool exit=false;
    cyg_flag_value_t efv, aux;
    //req = (request *)malloc(sizeof(request));
    //ack = (acknowledge*)malloc(sizeof(acknowledge));
    init_process();
    //init_ack(ack);
    //init_req(req);
    while(!exit){
        efv = cyg_flag_wait(&ef, 0x07 ,CYG_FLAG_WAITMODE_OR); //devolve o bit ativos no momento
        if ( efv & (aux=0x01)){
            // register
            cyg_flag_maskbits(&ef, aux);
            process();
        } if (efv & (aux=0x02)){
            // alarm
            cyg_flag_maskbits(&ef, aux);
            init_req(req);
            req->cmd=CODE_TRC;
            req->arg[0] = 10;
            cyg_mbox_put(user_com_channel_H, req);
            ack = cyg_mbox_get(com_user_channel_H);
            if (ack->error) exit=true;
            free(ack);
        } if (efv &(aux=0x04)) {//efv & 0x04 
            // user
            aux=0x04;
            cyg_flag_maskbits(&ef, aux);
            req_user = (request *) cyg_mbox_get(user_pro_channel_H);
            switch(req_user->cmd){
            case CODE_CPT:
                init_req(req);
                req->arg[0]=getAlarmPeriod();
                cyg_mbox_put(pro_user_channel_H, req);
                break;
            case CODE_MPT:
                setAlarmPeriod(req_user->arg[0]);
                activateAlarm();
                ack->error=false;
                cyg_mbox_put(pro_user_channel_H, ack);
                break;
            case CODE_CTTL:
                init_req(req);
                req->arg[0]=getTemperatureThreshold();
                req->arg[1]=getLuminosityThreshold();
                cyg_mbox_put(pro_user_channel_H, req);
                break;
            case CODE_DTTL:
                setTemperatureThreshold(req_user->arg[0]);
                setLuminosityThreshold(req_user->arg[1]);
                ack->error=false;   
                cyg_mbox_put(pro_user_channel_H, ack);
                break;
            case CODE_PR:
                init_req(req);
                process_local(req_user->arg, req);
                cyg_mbox_put(pro_user_channel_H, req);
                break;
            case CODE_EXIT:
                exit=true;
                break;
            }
            free(req_user);
        }
        //free(req);
        //free(ack);
        cyg_thread_exit();
    }
}
