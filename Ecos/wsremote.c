#include <stdio.h>
#include <cyg/kernel/kapi.h>

#define NTHREADS 4
#define STACKSIZE 4096
#define DEBUG 1

//Thread handling
static cyg_handle_t thread[NTHREADS];
static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];

//Threads functions
cyg_thread_entry_t main_processing;
cyg_thread_entry_t main_pic;
cyg_thread_entry_t main_monitor;

cyg_thread_entry_t main_periodic;

//Sycronization stuff
cyg_handle_t user_com_channel_H, com_user_channel_H; //mailbox between the user interface thread and the communication thread
cyg_mbox user_com_channel, com_user_channel;
cyg_handle_t pro_user_channel_H, user_pro_channel_H; //mailbox between the user interface thread and the processing thread
cyg_mbox pro_user_channel, user_pro_channel;
cyg_mutex_t stdin_mutex;
cyg_mutex_t local_mutex;

// functions and variables from other files
extern void cmd_ini (int, char** );
extern void monitor(void);
extern void pic(void);
extern void processing(void);
extern void init_local(void);
extern void init_pmem(void);
extern void list_pmem(void);



//PPP
extern void periodic();




/* we install our own startup routine which sets up
    threads and starts the scheduler */
void cyg_user_start(void){
  int i=0;

  cyg_mbox_create(&user_com_channel_H, &user_com_channel);
  cyg_mbox_create(&com_user_channel_H, &com_user_channel);
  cyg_mbox_create(&pro_user_channel_H, &pro_user_channel);
  cyg_mbox_create(&user_pro_channel_H, &user_pro_channel);

  cyg_mutex_init(&stdin_mutex);
  cyg_mutex_init(&local_mutex);

  cyg_thread_create(4, main_processing, (cyg_addrword_t) 0, "processing", (void *) stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
  cyg_thread_create(3, main_pic, (cyg_addrword_t) 0, "communication", (void *) stack[1], STACKSIZE, &thread[1], &thread_obj[1]);
  cyg_thread_create(20, main_monitor, (cyg_addrword_t) 0, "user", (void *) stack[2], STACKSIZE, &thread[2], &thread_obj[2]);
  
  //PPP
  //prioridade desta?!?
  cyg_thread_create(2, main_periodic, (cyg_addrword_t) 0, "periodic",
                    (void *) stack[3], STACKSIZE, &thread[3], &thread_obj[3]);

  cmd_ini(0, NULL);
  init_local();
  init_pmem(); //TODO: Delete this

  for(i = 0; i< NTHREADS; i++)
    cyg_thread_resume(thread[i]);

  cyg_thread_exit();
}

void main_pic(cyg_addrword_t data){
  pic();
}

void main_processing(cyg_addrword_t data){
  processing();
}

void main_monitor(cyg_addrword_t data){
  monitor();
}



//PPPP
extern cyg_io_handle_t serH;
void main_periodic(cyg_addrword_t data){

  ////////set to non-blocking mode (needed to set timeouts)///////
  cyg_uint32 blocking = 0;
  cyg_uint32 len = sizeof(blocking);

  if (ENOERR != cyg_io_set_config(serH, CYG_IO_SET_CONFIG_READ_BLOCKING ,&blocking, &len))
    printf("error setting serH io_read to non-blocking - wsremote.c\n");
  ///////////////////////////////////////////////////////////////

  /////////////set the timeouts/////////////////////////////////
  cyg_can_timeout_info_t timeouts;
  len = sizeof(timeouts);
  
  timeouts.rx_timeout = 0; //timeout for reads
  timeouts.tx_timeout = 0; // timeout for writes


  if (ENOERR != cyg_io_set_config(serH, CYG_IO_SET_CONFIG_CAN_TIMEOUT ,&timeouts, &len))
    printf("error setting serH io_read to non-blocking - wsremote.c\n");
  //////////////////////////////////////////////////////////////

  periodic();
}
