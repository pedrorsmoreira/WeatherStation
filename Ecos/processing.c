#include "structure.h"

//Sycronization stuff
extern cyg_handle_t user_pro_channel_H;
extern cyg_handle_t pro_user_channel_H;
extern void init_req(request *);
extern void init_ack(acknowledge*);
extern void process_local(int [], request *);

static request * req_user = NULL;
static request * req = NULL;
static acknowledge * ack = NULL;

typedef struct Process{
    int transference_period;
    int temperature_threshold;
    int luminosity_threshold;
} process;

static process pro;

void processing(void);
void init_process(void);

void processing(void){ //TODO: Á medida que as trasnferencias sao feitas, deve imprimir cenas no ecrã com os thresholds
    bool exit=false;
    req = (request *)malloc(sizeof(request));
    ack = (acknowledge*)malloc(sizeof(acknowledge));
    init_process();
    init_ack(ack);
    init_req(req);
    while(!exit){
        req_user = (request *) cyg_mbox_get(user_pro_channel_H);
        switch(req_user->cmd){
        case CODE_CPT:
            init_req(req);
            req->arg[0]=pro.transference_period;
            cyg_mbox_put(pro_user_channel_H, req);
            break;
        case CODE_MPT:
            pro.transference_period=req_user->arg[0];
            ack->error=false;
            cyg_mbox_put(pro_user_channel_H, ack);
            break;
        case CODE_CTTL:
            init_req(req);
            req->arg[0]=pro.temperature_threshold;
            req->arg[1]=pro.luminosity_threshold;
            cyg_mbox_put(pro_user_channel_H, req);
            break;
        case CODE_DTTL:
            pro.temperature_threshold=req_user->arg[0];
            pro.luminosity_threshold=req_user->arg[1];
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
    }
    free(req);
    free(ack);
    cyg_thread_exit();
}

void init_process(){
    pro.transference_period=0;
    pro.temperature_threshold=0;
    pro.luminosity_threshold=0;
}
