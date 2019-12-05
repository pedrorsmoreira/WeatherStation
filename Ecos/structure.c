#include "structure.h"

static local memory;
static pic_memory pmem;

//sync stuff
extern cyg_mutex_t stdin_mutex;
extern cyg_mutex_t local_mutex;

//functions declaration
void init_local(void);
void add_local(int, int, int, int, int);
bool is_full(void);
bool list_local(int, int);
void process_local(int [], request *);
void delete_local(void);
void init_pmem(void);
void add_pmem(int, int, int, int, int);
void init_reg(buffer *);
void add_reg(buffer *, int, int, int, int, int);
void copy_reg(buffer*, buffer*);
void init_req(request*);
void init_ack(acknowledge*);

void init_local(void){
    int i;
    cyg_mutex_lock(&local_mutex);
    memory.nr=0;
    memory.iread=0;
    memory.iwrite=0;
    for (i=0; i<NRBUF; i++)
        init_reg(&(memory.reg[i]));
    cyg_mutex_unlock(&local_mutex);

    //aqui nao e preciso porque vai desaparecer proteger
    add_local(21,1,1,40,53);
    add_local(22,2,2,34,51);
    add_local(25,4,3,32,15);
    add_local(22,2,4,42,15);
    add_local(23,2,5,43,52);
    add_local(25,3,6,35,54);
    add_local(26,0,7,26,55);
    add_local(28,0,8,34,55);
    add_local(29,0,9,52,45);
    add_local(37,1,10,42,53);
    add_local(35,1,11,36,25);
    add_local(34,2,12,23,54);
    add_local(37,3,13,45,25);
    add_local(35,2,14,24,53);
    add_local(36,4,15,44,45);
    add_local(38,3,16,24,52);
    add_local(39,2,17,14,52);
}

void add_local( int temperature, int luminosity, int hour, int minute, int second){
   cyg_mutex_lock(&local_mutex); 
    if (!is_full())
        memory.nr++;
    add_reg(&(memory.reg[memory.iwrite]), temperature, luminosity, hour, minute, second);
    memory.iwrite=(memory.iwrite+1)%NRBUF;
    cyg_mutex_unlock(&local_mutex);
}

//não é preciso proteger pq qd é chamado já esta protegido
bool is_full(void){
    return !(memory.nr == memory.iwrite);
}

bool list_local(int n, int index){
    int c, i;
    cyg_mutex_lock(&local_mutex);
    if(index>=memory.nr){
        cyg_mutex_unlock(&local_mutex);
        return false;
    }
    for(c=0, i=is_full()?(memory.iwrite+index)%NRBUF:index ; c<n && (i!=memory.iwrite || c==0); i=(i+1)%NRBUF, c++){
        cyg_mutex_lock(&stdin_mutex);
        printf("%d - [%d:%d:%d] t=%d l=%d\n", 
            i, memory.reg[i].hour, memory.reg[i].minute, memory.reg[i].second, memory.reg[i].temperature, memory.reg[i].luminosity);
        cyg_mutex_unlock(&stdin_mutex);
    }
    cyg_mutex_unlock(&local_mutex); //TODO: verificar que não faz deadlock
    return true;
}

void info_local(void){
    cyg_mutex_lock(&local_mutex);
    cyg_mutex_lock(&stdin_mutex);
    printf("NRBUF: %d\nnr: %d\niread: %d\niwrite: %d\n",
            NRBUF, memory.nr, memory.iread, memory.iwrite);
    cyg_mutex_unlock(&stdin_mutex);
    cyg_mutex_unlock(&local_mutex);
}

void delete_local(void){
    int i;
    cyg_mutex_lock(&local_mutex);
    int n = is_full() ? NRBUF : memory.iwrite;
    for(i=0; i<n; i++){
        memory.reg[i].hour=0;
        memory.reg[i].minute=0;
        memory.reg[i].second=0;
        memory.reg[i].temperature=0;
        memory.reg[i].luminosity=0;
    }
    memory.iread=0;
    memory.iwrite=0;
    memory.nr=0;
   cyg_mutex_unlock(&local_mutex); 
}

void process_local(int argv[], request * req){
    int c, i, n;
    int h1=0 , m1=0, s1=0, h2=23, m2=59, s2=59;
    int max_temperature = 0, min_temperature = 0;
    int max_luminosity = 0, min_luminosity = 0 ;
    float avg_temperature = 0;
    float avg_luminosity = 0 ;
    bool start;
    if(argv[0]>0){
        h1=argv[1];
        m1=argv[2];
        s1=argv[3];
    }
    if (argv[0]==2){
        h2=argv[4];
        m2=argv[5];
        s2=argv[6];
    }
    printf("%d:%d:%d - %d:%d:%d\n", h1, m1, s1, h2, m2, s2);
    cyg_mutex_lock(&local_mutex);
    if  (memory.nr==0){  //nao tem registos na memoria local
        req->arg[0]=-1;
        return;
    }
    start = false;
    n=0;
    for(c=0, i=is_full() ? memory.iwrite : 0 ; i!=memory.iwrite || c==0; i=(i+1)%NRBUF, c++){
        if (start){
            if (memory.reg[i].hour>=h2 && memory.reg[i].minute>=m2 && memory.reg[i].second>=s2) break;
            if (min_temperature>memory.reg[i].temperature) min_temperature=memory.reg[i].temperature;
            if (max_temperature<memory.reg[i].temperature) max_temperature=memory.reg[i].temperature;
            avg_temperature += memory.reg[i].temperature;
            if (min_luminosity>memory.reg[i].luminosity) min_luminosity=memory.reg[i].luminosity;
            if (max_luminosity<memory.reg[i].luminosity) max_luminosity=memory.reg[i].luminosity;
            avg_luminosity += memory.reg[i].luminosity;
            n++;
        } else if (memory.reg[i].hour>=h1 && memory.reg[i].minute>=m1 && memory.reg[i].second>=s1){
            min_temperature = memory.reg[i].temperature;
            max_temperature = min_temperature;
            max_luminosity = memory.reg[i].luminosity;
            min_luminosity = max_luminosity;
            avg_temperature = min_temperature;
            avg_luminosity = min_luminosity;
            n=1;
            start=true; 
        }
    }
    cyg_mutex_unlock(&local_mutex);
    if (n==0){ //nao ha registos no intervalo
        req->arg[0]=-2; 
        return;
    }
    req->arg[0]=min_temperature;
    req->arg[1]=max_temperature;
    req->arg[2]=avg_temperature/n;
    req->arg[3]=min_luminosity;
    req->arg[4]=max_luminosity;
    req->arg[5]=avg_luminosity/n;
}

void init_pmem(void){
    int i;
    pmem.nr=0;
    pmem.iread=0;
    pmem.iwrite=0;
    for (i=0; i<NREG; i++)
        init_reg(&(pmem.reg[i]));
}

void add_pmem(int temperature, int luminosity, int hour, int minute, int second){
    add_reg(&(pmem.reg[pmem.iwrite]), temperature, luminosity, hour, minute, second);
    pmem.iwrite++;
    if (pmem.iwrite == NREG) pmem.iwrite=0;
    else pmem.nr++;
}

// PARA DEBUG ONLY
//void list_pmem(void){
//    int i;
//    printf("nr:%d\n",pmem.nr);
//    printf("iread:%d\n",pmem.iread);
//    printf("iwrite:%d\n",pmem.iwrite);
//    for(i=0; i<NREG-1; i=i+2)
//        printf("%d:[%d:%d:%d] %d, %d\t\t%d:[%d:%d:%d] %d, %d\n", 
//                i, pmem.reg[i].hour, pmem.reg[i].minute, pmem.reg[i].second, pmem.reg[i].temperature, pmem.reg[i].luminosity,
//                i+1, pmem.reg[i+1].hour, pmem.reg[i+1].minute, pmem.reg[i+1].second, pmem.reg[i+1].temperature, pmem.reg[i+1].luminosity);
//}

void init_reg(buffer* buf){
    buf->temperature = 0;
    buf->luminosity = 0;
    buf->hour = 0;
    buf->minute = 0;
    buf->second = 0;
}

void add_reg(buffer * buf, int temperature, int luminosity, int hour, int minute, int second){
    buf->temperature = temperature;
    buf->luminosity = luminosity;
    buf->hour = hour;
    buf->minute = minute;
    buf->second = second;
}

void copy_reg(buffer* buf1, buffer* buf2){
    buf1->temperature = buf2->temperature;
    buf1->luminosity = buf2->luminosity;
    buf1->hour = buf2->hour;
    buf1->minute = buf2->minute;
    buf1->second = buf2->second;
}

/*-------------------------------------------------------------------------+
| Function: variable_init - clean das estruturas de request/ack
+--------------------------------------------------------------------------*/ 
void init_req(request *r){
    int i;
    for (i=0; i<N_ARGS; i++)
        r->arg[i]=0;
    r->cmd=0;
}

void init_ack(acknowledge* ack){
    ack->error=false;
}









/////////////////////alarm///////////////////////////

static alarmStuff alarm;

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
    cyg_alarmextern extern extern extern extern _create(alarmCounter, alarmfn,
    (cyg_addrword_t) 0, &alarm.id, &alarm_);
}

void activateAlarm(){
    int period;
    cyg_handle_t id;

    if (alarm.period == 0) return;

    cyg_mutex_lock(&alarm.mutex);
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
    alarm.period = period;
    cyg_mutex_unlock(&alarm.mutex);
}