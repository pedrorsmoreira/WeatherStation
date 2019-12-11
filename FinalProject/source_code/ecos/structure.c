#include "structure.h"

//useful variables (global but only used in this file)
static local memory;
static cyg_uint8 pro_temperature;
static cyg_uint8 pro_luminosity;

//syncronization variables
extern cyg_mutex_t stdin_mutex;
extern cyg_mutex_t local_mutex;
extern cyg_flag_t ef;

//functions declaration
void init_local(void);
void add_local(cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8);
bool is_full(void);
bool list_local(cyg_uint8, cyg_uint8);
void process_local(cyg_uint8 [], request *);
void delete_local(void);
void init_pmem(void);
void add_pmem(cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8);
void init_reg(buffer *);
void add_reg(buffer *, cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8, cyg_uint8);
void copy_reg(buffer*, buffer*);
void init_req(request*);
void init_ack(acknowledge*);
void init_process(void);
void process(void);
void process_register(void);
void setTemperatureThreshold(cyg_uint8);
void setLuminosityThreshold(cyg_uint8);
cyg_uint8 getTemperatureThreshold(void);
cyg_uint8 getLuminosityThreshold(void);

//LOCAL MEMORY 

//initializa the local memory function
void init_local(void){
    cyg_uint8 i;
    cyg_mutex_lock(&local_mutex);
    memory.nr=0;
    memory.iread=0;
    memory.iwrite=0;
    for (i=0; i<NRBUF; i++)
        init_reg(&(memory.reg[i]));
    cyg_mutex_unlock(&local_mutex);
}

//add a register to the local memory
void add_local( cyg_uint8 hour, cyg_uint8 minute, cyg_uint8 second, cyg_uint8 temperature, cyg_uint8 luminosity){
   cyg_mutex_lock(&local_mutex); 
    if (!is_full())
        memory.nr++;
    add_reg(&(memory.reg[memory.iwrite]), temperature, luminosity, hour, minute, second);
    memory.iwrite=(memory.iwrite+1)%NRBUF;
    cyg_mutex_unlock(&local_mutex);
}

//check if local ring buffer is full
bool is_full(void){
    return !(memory.nr == memory.iwrite);
}

//list the register
bool list_local(cyg_uint8 n, cyg_uint8 index){
    cyg_uint8 c, i;
    cyg_mutex_lock(&local_mutex);
    if(index>=memory.nr){
        cyg_mutex_unlock(&local_mutex);
        return false;
    }
    for(c=0, i=is_full()?(memory.iwrite+index)%NRBUF:index ; c<n && (i!=memory.iwrite || c==0); i=(i+1)%NRBUF, c++){
        cyg_mutex_lock(&stdin_mutex);
        printf("%u - [%u:%u:%u] t=%u l=%u\n", 
            i, memory.reg[i].hour, memory.reg[i].minute, memory.reg[i].second, memory.reg[i].temperature, memory.reg[i].luminosity);
        cyg_mutex_unlock(&stdin_mutex);
    }
    cyg_mutex_unlock(&local_mutex);
    return true;
}

//list the local information
void info_local(void){
    cyg_mutex_lock(&local_mutex);
    cyg_mutex_lock(&stdin_mutex);
    printf("NRBUF: %d\nnr: %d\niread: %d\niwrite: %d\n",
            NRBUF, memory.nr, memory.iread, memory.iwrite);
    cyg_mutex_unlock(&stdin_mutex);
    cyg_mutex_unlock(&local_mutex);
}

//delete local memory
void delete_local(void){
    cyg_uint8 i;
    cyg_mutex_lock(&local_mutex);
    cyg_uint8 n = is_full() ? NRBUF : memory.iwrite;
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

//Process information request: "pr" command by user
void process_local(cyg_uint8 argv[], request * req){
    cyg_uint8 c, i, n;
    cyg_uint8 h1=0 , m1=0, s1=0, h2=23, m2=59, s2=59;
    cyg_uint8 max_temperature = 0, min_temperature = 0;
    cyg_uint8 max_luminosity = 0, min_luminosity = 0 ;
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
        req->arg[0]=24;
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
        req->arg[0]=25; 
        return;
    }
    req->arg[0]=min_temperature;
    req->arg[1]=max_temperature;
    req->arg[2]=avg_temperature/n;
    req->arg[3]=min_luminosity;
    req->arg[4]=max_luminosity;
    req->arg[5]=avg_luminosity/n;
}


//REGISTER SPECIFIC FUNCTIONS

//inti register
void init_reg(buffer* buf){
    buf->temperature = 0;
    buf->luminosity = 0;
    buf->hour = 0;
    buf->minute = 0;
    buf->second = 0;
}

//add a new register to a buffer
void add_reg(buffer * buf, cyg_uint8 temperature, cyg_uint8 luminosity, cyg_uint8 hour, cyg_uint8 minute, cyg_uint8 second){
    buf->temperature = temperature;
    buf->luminosity = luminosity;
    buf->hour = hour;
    buf->minute = minute;
    buf->second = second;
}

//copy register from buffer
void copy_reg(buffer* buf1, buffer* buf2){
    buf1->temperature = buf2->temperature;
    buf1->luminosity = buf2->luminosity;
    buf1->hour = buf2->hour;
    buf1->minute = buf2->minute;
    buf1->second = buf2->second;
}

//MAILBOX REQUEST/REPLY STRUCT SPECIFIC FUNCTIONS

//Malloc of the request variable
void init_req(request *r){
    r=(request*)malloc(sizeof(request));
    cyg_uint8 i;
    for (i=0; i<N_ARGS; i++)
        r->arg[i]=0;
    r->cmd=0;
}

//MAlloc of the ack variable
void init_ack(acknowledge* ack){
    ack=(acknowledge*)malloc(sizeof(acknowledge));
    ack->error=false;
}

//PROCESS THREAD SPECIFC FUNCTIONS

void init_process(void){
	pro_temperature=TMP_THR;
	pro_luminosity=LUM_THR;
}

void process(void){
	cyg_mutex_lock(&local_mutex);
	while(memory.iread != memory.iwrite)
		process_register();
	cyg_mutex_unlock(&local_mutex);
} 

void process_register(void){
	if (memory.reg[memory.iread].temperature>=pro_temperature){
		cyg_mutex_lock(&stdin_mutex);
		printf("Warning - %d:%d:%d - temperature is %d\n",
			memory.reg[memory.iread].hour,
			memory.reg[memory.iread].minute,
			memory.reg[memory.iread].second,
			memory.reg[memory.iread].temperature);
		cyg_mutex_unlock(&stdin_mutex);	
	} if (memory.reg[memory.iread].luminosity>=pro_luminosity){
		cyg_mutex_lock(&stdin_mutex);
		printf("Warning - %d:%d:%d - luminosity is %d\n",
			memory.reg[memory.iread].hour,
			memory.reg[memory.iread].minute,
			memory.reg[memory.iread].second,
			memory.reg[memory.iread].luminosity);
		cyg_mutex_unlock(&stdin_mutex);	
	}
	memory.iread=(memory.iread+1)%NRBUF;	
}

void setTemperatureThreshold(cyg_uint8 thr){pro_temperature=thr;}
void setLuminosityThreshold(cyg_uint8 thr){pro_luminosity=thr;}
cyg_uint8 getTemperatureThreshold(void){return pro_temperature;}
cyg_uint8 getLuminosityThreshold(void){return pro_luminosity;}

//ALARM FUNCTIONS AND VARIABLEs

static alarmStuff alarm;

//event flag
extern cyg_flag_t ef;

cyg_handle_t clk;
unsigned uselesss;
cyg_handle_t alarmCounter;
cyg_alarm alarm_;

//alarmHandler
void alarmfn(cyg_handle_t alarmH, cyg_addrword_t data){
    cyg_flag_value_t efv=0x02;
    cyg_flag_setbits(&ef, efv);
}

void alarm_init(void){
    cyg_mutex_init(&alarm.mutex);
    alarm.issued = false;
    alarm.period = 0;
    clk = cyg_real_time_clock();
    cyg_clock_to_counter(clk, &alarmCounter);
    cyg_alarm_create(alarmCounter, alarmfn,
    (cyg_addrword_t) &uselesss, &alarm.id, &alarm_);
}

void activateAlarm(void){
    cyg_uint8 period;
    cyg_handle_t id;
    if (alarm.period == 0) return;
    cyg_mutex_lock(&alarm.mutex);
    period = alarm.period;
    id = alarm.id;
    cyg_mutex_unlock(&alarm.mutex);
    cyg_alarm_initialize(alarm.id, cyg_current_time(), (cyg_tick_count_t) period*10*60);
}

void deactivateAlarm(void){
    cyg_handle_t id;
    cyg_mutex_lock(&alarm.mutex);
    id = alarm.id;
    alarm.period = 0;
    cyg_mutex_unlock(&alarm.mutex);
    cyg_alarm_disable(id);
}

bool IsAlarmActive(void){
    bool ret;
    cyg_mutex_lock(&alarm.mutex);
    ret = alarm.period > 0;
    cyg_mutex_unlock(&alarm.mutex);
    return ret;
}

bool IsAlarmIssued(void){
    bool ret;
    cyg_scheduler_lock();
    ret = alarm.issued;
    alarm.issued = 0;
    cyg_scheduler_unlock();
    return ret;
}

cyg_uint8 getAlarmPeriod(void){
    bool ret;
    cyg_mutex_lock(&alarm.mutex);
    ret = alarm.period;
    cyg_mutex_unlock(&alarm.mutex);
    return ret;
}

void setAlarmPeriod(cyg_uint8 period){
	if (period == 0)
	deactivateAlarm();
	else{
	    cyg_mutex_lock(&alarm.mutex);
	    alarm.period = period;
	    cyg_mutex_unlock(&alarm.mutex);
	}
}
