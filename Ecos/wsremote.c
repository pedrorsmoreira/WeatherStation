#include <stdio.h>
#include <cyg/kernel/kapi.h>

#include "structure.h"

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
cyg_thread_entry_t main_read_pic;

//Sycronization stuff
cyg_handle_t user_com_channel_H, com_user_channel_H; //mailbox between the user interface thread and the communication thread
cyg_mbox user_com_channel, com_user_channel;
cyg_handle_t pro_user_channel_H, user_pro_channel_H; //mailbox between the user interface thread and the processing thread
cyg_mbox pro_user_channel, user_pro_channel;
cyg_mutex_t stdin_mutex;
cyg_mutex_t local_mutex;
cyg_io_handle_t serH;

// functions and variables from other files
extern void cmd_ini (int, char** );
extern void monitor(void);
extern void write_pic(void);
extern void processing(void);
extern void read_pic(void);
extern void init_local(void);
extern void init_pmem(void);
extern void list_pmem(void);
extern void alarm_init(void);

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

  //alarm set up
  alarm_init();

  cyg_thread_create(4, main_processing, (cyg_addrword_t) 0, "processing", (void *) stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
  cyg_thread_create(3, main_write_pic, (cyg_addrword_t) 0, "communicationWrite", (void *) stack[1], STACKSIZE, &thread[1], &thread_obj[1]);
  cyg_thread_create(20, main_monitor, (cyg_addrword_t) 0, "user", (void *) stack[2], STACKSIZE, &thread[2], &thread_obj[2]);
  cyg_thread_create(2, main_read_pic, (cyg_addrword_t) 0, "communicationRead",
                    (void *) stack[3], STACKSIZE, &thread[3], &thread_obj[3]);

  //cmd_ini(0, NULL);
  init_local();
  init_pmem(); //TODO: Delete this

  for(i = 0; i< NTHREADS; i++)
    cyg_thread_resume(thread[i]);

  cyg_thread_exit();
}

void main_write_pic(cyg_addrword_t data){
  write_pic();
}

void main_processing(cyg_addrword_t data){
  processing();
}

void main_monitor(cyg_addrword_t data){
  cmd_ini(0, NULL);
  monitor();
}

void main_read_pic(cyg_addrword_t data){
  read_pic();
}
