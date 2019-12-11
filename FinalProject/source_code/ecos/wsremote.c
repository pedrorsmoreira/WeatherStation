#include <stdio.h>
#include <cyg/kernel/kapi.h>

#include "structure.h"

#define NTHREADS 4
#define STACKSIZE 4096

//Thread handling
static cyg_handle_t thread[NTHREADS];
static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];

//Threads functions
cyg_thread_entry_t main_processing;
cyg_thread_entry_t main_pic;
cyg_thread_entry_t main_monitor;
cyg_thread_entry_t main_read_pic;
cyg_thread_entry_t main_write_pic;

//Sycronization variables
cyg_handle_t user_com_channel_H, com_user_channel_H; //mailbox between the user interface thread and the communication thread
cyg_mbox user_com_channel, com_user_channel;
cyg_handle_t pro_user_channel_H, user_pro_channel_H; //mailbox between the user interface thread and the processing thread
cyg_mbox pro_user_channel, user_pro_channel;
cyg_mutex_t stdin_mutex; 
cyg_mutex_t local_mutex; //local memory mutex
cyg_io_handle_t serH;
cyg_flag_t ef; //event flag for the processing task

// functions and variables from other files
extern void cmd_ini (cyg_uint8, char** );
extern void monitor(void);
extern void write_pic(void);
extern void processing(void);
extern void read_pic(void);
extern void init_local(void);
extern void init_pmem(void);
extern void list_pmem(void);
extern void alarm_init(void);

/* MAIN - setups and thread inicialization */
int main(void){
  cyg_uint8 i=0;

  // Init of the syncronization mechanisms
  cyg_mutex_init(&stdin_mutex);
  cyg_mutex_init(&local_mutex);
  cyg_mbox_create(&user_com_channel_H, &user_com_channel);
  cyg_mbox_create(&com_user_channel_H, &com_user_channel);
  cyg_mbox_create(&pro_user_channel_H, &pro_user_channel);
  cyg_mbox_create(&user_pro_channel_H, &user_pro_channel);
  cyg_flag_init(&ef);

  //alarm set up
  alarm_init();

  //serial set up
  cmd_ini(0, NULL);

  //thread creation
  cyg_thread_create(3, main_processing, (cyg_addrword_t) 0, "processing", (void *) stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
  cyg_thread_create(4, main_write_pic, (cyg_addrword_t) 0, "communicationWrite", (void *) stack[1], STACKSIZE, &thread[1], &thread_obj[1]);
  cyg_thread_create(5, main_monitor, (cyg_addrword_t) 0, "user", (void *) stack[2], STACKSIZE, &thread[2], &thread_obj[2]);
  cyg_thread_create(2, main_read_pic, (cyg_addrword_t) 0, "communicationRead",
                    (void *) stack[3], STACKSIZE, &thread[3], &thread_obj[3]);

  //set up of the local memory
  init_local();

  //starting threads
  for(i = 0; i< NTHREADS; i++)
    cyg_thread_resume(thread[i]);

	return 0;
}

/* MAIN_WRITE_PIC - thread that writes in the PIC */
void main_write_pic(cyg_addrword_t data){
  write_pic();
}

/* MAIN_PROCESSING - thread that processes the data */
void main_processing(cyg_addrword_t data){
  processing();
}

/* MAIN_MONITOR - thread responsible for the user task*/
void main_monitor(cyg_addrword_t data){
  monitor();
}

/* MAIN_READ_PIC - thread that reads values from the PIC*/
void main_read_pic(cyg_addrword_t data){
  read_pic();
}
