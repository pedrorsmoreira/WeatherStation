#include <cyg/io/io.h>
#include "structure.h"

// syncronization variables
extern cyg_handle_t com_user_channel_H;
extern cyg_handle_t user_com_channel_H;
extern cyg_handle_t pro_user_channel_H;
extern cyg_handle_t user_pro_channel_H;
extern cyg_mutex_t stdin_mutex;
extern cyg_io_handle_t serH;
extern cyg_flag_t ef;

// general varaibles for the user thread
static cyg_flag_value_t efv=0x04;
static Cyg_ErrNo err;
static request *req = NULL;
static request *req_other = NULL;
static acknowledge *ack_other = NULL;

// functions from other files
extern void init_req(request*);
extern void init_ack(acknowledge*);
extern void info_local(void);
extern bool list_local(int, int);
extern void delete_local(void);

/*-------------------------------------------------------------------------+
| Function: cmd_ini - inicializar dispositivo
+--------------------------------------------------------------------------*/ 
void cmd_ini(int argc, char **argv){
  if ((argc > 1) && (argv[1][0] = '1'))
    err = cyg_io_lookup("/dev/ser1", &serH);
  else err = cyg_io_lookup("/dev/ser0", &serH);
}

//function to report error to the user
void not_valid(void){
  cyg_mutex_lock(&stdin_mutex);
  printf("Command not valid.\n");
  cyg_mutex_unlock(&stdin_mutex);
}

//function to quit the program
void cmd_sair (int argc, char **argv){
  init_req(req);
  req->cmd=CODE_EXIT;
  cyg_mbox_put(user_com_channel_H, req); 
  cyg_mutex_lock(&stdin_mutex);
  printf("Exiting\n.");
  cyg_mutex_unlock(&stdin_mutex);
  cyg_thread_exit();
}

// when user requests the clock
void cmd_rc ( int argc, char **argv) {
  if (argc==1){
    init_req(req); //alloc the request
    req->cmd=CODE_RC;
    cyg_mbox_put(user_com_channel_H, req); //send request to the comunication task
    req_other = (request *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 10000); //waits for the answer
    if (req_other == NULL) //if timeout
      return;
    cyg_mutex_lock(&stdin_mutex);
    printf("clock: %u:%u:%u [hh:mm:ss]\n", req_other->arg[0], req_other->arg[1], req_other->arg[2]);
    cyg_mutex_unlock(&stdin_mutex);
    free(req_other); //frees the reply
  } else not_valid();
}

//when the user wants to set the clock
void cmd_sc (int argc, char **argv ){
  int aux[3], i;
  bool flag = true;
  if ( argc == 4 ){ //check if 3 arguments (h,m,s)
    for (i=1; i<4; i++){
      if (sscanf( argv[i], "%d", &(aux[i-1])) != 1 || aux[i-1]<0){ //check arguments
        flag = false;
        break;
      }
    }
  } else flag = false;
  if (flag){
    if ( aux[0] < 23 && aux[1] < 59 && aux[2] < 59){ //check time integrity
      init_req(req); //allocs request
      req->cmd=CODE_SC;
      for (i=0; i<3; i++) req->arg[i]=aux[i]; //put the time in the arguments
      cyg_mbox_put(user_com_channel_H, req); //send request to communication task
      ack_other = (acknowledge *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //waits for the ack
      if (ack_other == NULL) //if timeout
        return; 
      //shows to the user
      cyg_mutex_lock(&stdin_mutex);
      printf("%s.\n", ack_other->error ? "Not possible to change the clock" : "Clock changed");
      cyg_mutex_unlock(&stdin_mutex);
      free(ack_other); //frees reply
    } else not_valid();
  } else not_valid();
}

//when user wants to read the time and luminosity
void cmd_rtl (int argc, char **argv ){
  if (argc==1){
    init_req(req); //allocs the request
    req->cmd=CODE_RTL;
    cyg_mbox_put(user_com_channel_H, req); //send request
    req_other = (request *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //waits for info
    if (req_other == NULL) //if timeout
      return;
    //show to the user
    cyg_mutex_lock(&stdin_mutex);
    printf("Temperature: %d\nLuminosity: %d\n", req_other->arg[0], req_other->arg[1]);
    cyg_mutex_unlock(&stdin_mutex);
    free(req_other); //frees reply
  } else not_valid();
}

//when user wants to "rp"
void cmd_rp (int argc, char **argv ){
   if (argc==1){
    init_req(req); //allocs the request
    req->cmd=CODE_RP;
    cyg_mbox_put(user_com_channel_H, req); //send request
    req_other = (request *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //waits for info
    if (req_other == NULL) //if timeout
      return;
    //show to user
    cyg_mutex_lock(&stdin_mutex);
    printf("PMON: %d\nTALA: %d\n", req_other->arg[0], req_other->arg[1]);
    cyg_mutex_unlock(&stdin_mutex);
    free(req_other); //free reply
  } else not_valid();
}

//when the user wants to "mmp"
void cmd_mmp (int argc, char **argv ){
  int aux;
  if ( argc == 2 && sscanf( argv[1], "%d", &(aux)) == 1 && aux>=0 && aux<100) { //check arguments
    init_req(req); //allocs request
    req->cmd=CODE_MMP;
    req->arg[0]=aux; //put the period in the arguments
    cyg_mbox_put(user_com_channel_H, req); //send request
    ack_other = (acknowledge *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //waits for ack
    if (ack_other == NULL) //if timeout
      return; 
    // show to user 
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error? "Not possible to change the monitoring period": "Monitoring period changed");
    cyg_mutex_unlock(&stdin_mutex);
    free(req_other); //free reply
  } else not_valid();
}

//when user requests "mta"
void cmd_mta (int argc, char **argv ){
  int aux;
  if ( argc == 2 && sscanf( argv[1], "%d", &(aux)) == 1 && aux>=0 && aux<=60){
    init_req(req); //allocs request
    req->cmd=CODE_MTA;
    req->arg[0]=aux; //put the time alarm in the arguments
    cyg_mbox_put(user_com_channel_H, req); //send request
    ack_other = (acknowledge *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000);  //wait for anwser
    if (ack_other == NULL) //if timeout
      return; 
    //show to suer
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error? "Not possible to change the time alarm": "Time alarm changed");
    cyg_mutex_unlock(&stdin_mutex);
    free(ack_other); //free reply
  } else not_valid();
}

//when user requests "ra"
void cmd_ra (int argc, char **argv ){
    if (argc==1){
    init_req(req); //allocs request
    req->cmd=CODE_RA;
    cyg_mbox_put(user_com_channel_H, req); //send request
    req_other = (request *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //waits for anwser
    if (req_other == NULL) //if timeout
      return;
    //shows user
    cyg_mutex_lock(&stdin_mutex);
    printf("Temperature Alarm: %d\nLuminosity Alarm: %d\nActive? %s\n", 
      req_other->arg[0], req_other->arg[1],  req_other->arg[2]? "yes":"no" );
    cyg_mutex_unlock(&stdin_mutex);
    free(req_other); //free reply
  } else not_valid();
}

//when user request "dtl"
void cmd_dtl (int argc, char **argv ){
  int aux[2], i, limits[] = {50, 3};
  bool flag = true;
  if ( argc == 3 ){ //check if 2 arguments (t and l)
    for (i=1; i<3; i++){
      if (sscanf( argv[i], "%d", &(aux[i-1])) != 1 || aux[i-1]<0 || aux[i-1] > limits[i-1]){
        flag = false;
        break;
      }
    }
  } else flag = false;
  if (flag){ // arguemtns are aceptable
    init_req(req); //malloc
    req->cmd=CODE_DTL;
    for (i=0; i<2; i++) req->arg[i]=aux[i]; //put the alarms in the arguments
    cyg_mbox_put(user_com_channel_H, req); // send request
    ack_other = (acknowledge *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //waits  for reply
    if (ack_other == NULL) //if timeout
      return;
    //show user
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error ? "Not possible to change the parameters" : "Alarms changed");
    cyg_mutex_unlock(&stdin_mutex);
    free(ack_other); //Free
  } else not_valid();
}

//when user requests "aa"
void cmd_aa (int argc, char **argv ){
  int aux;
  if ( argc == 2 && sscanf( argv[1], "%d", &(aux)) == 1 && (aux==0 || aux==1)){ //check arguments
    init_req(req); //malloc
    req->cmd=CODE_AA;
    req->arg[0]=aux; //put the alarm state in the arguments
    cyg_mbox_put(user_com_channel_H, req); //send command
    ack_other = (acknowledge *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //wait ack
    if (ack_other == NULL) //timeout
      return; 
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error? "Not possible to change the alarm state": "Alarm state changed");
    cyg_mutex_unlock(&stdin_mutex);
    free(ack_other); //free
  } else not_valid();
}

//when user requests "ir"
void cmd_ir (int argc, char **argv ){
  if (argc==1){
    init_req(req); //malloc
    req->cmd=CODE_IR;
    cyg_mbox_put(user_com_channel_H, req); //send request
    req_other = (request *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //wait for reply
    if (req_other == NULL) //timeout
      return; 
    cyg_mutex_lock(&stdin_mutex);
    printf("NREG: %d\nnr: %d\niread: %d\niwrite: %d\n", req_other->arg[0], req_other->arg[1], req_other->arg[2], req_other->arg[3]);
    cyg_mutex_unlock(&stdin_mutex);
    free(req_other); //free
  } else not_valid();
}
void cmd_trc (int argc, char **argv ){
  int aux;
  if (argc ==2 && sscanf(argv[1], "%d", &(aux)) == 1 && aux>0 && aux<11){ 
    init_req(req); //malloc
    req->cmd=CODE_TRC;
    req->arg[0]=aux; //put number of reg in the arg
    cyg_mbox_put(user_com_channel_H, req); //Send request
    ack_other = (acknowledge *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //waits for reply
    if (ack_other == NULL) //timeout
      return; 
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error ? "Not possible to transfer registers" : "Registers transferred");
    cyg_mutex_unlock(&stdin_mutex);
    free(ack_other);
  } else not_valid();
}
void cmd_tri (int argc, char **argv ){
  int aux[2];
  if (argc ==3 && sscanf(argv[1], "%d", &(aux[0])) == 1 && aux[0]>0 && aux[0]<11
               && sscanf(argv[2], "%d", &(aux[1])) == 1 && aux[1]>=0 && aux[1]<NREG){
    init_req(req); //malloc
    req->cmd=CODE_TRI;
    req->arg[0]=aux[0]; //put the number of register in the request
    req->arg[1]=aux[1]; //put index in the request
    cyg_mbox_put(user_com_channel_H, req); //send request
    ack_other = (acknowledge *) cyg_mbox_timed_get(com_user_channel_H, cyg_current_time() + 1000); //wait for reply
    if (ack_other == NULL) //timeout
      return;
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error ? "Not possible to transfer registers" : "Registers trasnferred");
    cyg_mutex_unlock(&stdin_mutex);
    free(ack_other); //Free
  } else not_valid();
}

// LOCAL MEMORY
//when user request "irl"
void cmd_irl (int argc, char **argv ){
  if (argc==1) info_local();
  else not_valid();
}
//when user request "lr"
void cmd_lr (int argc, char **argv ){
  int aux[2];
  if (argc ==3 && sscanf(argv[1], "%d", &(aux[0])) == 1 && aux[0]>0 && aux[0]<NRBUF
               && sscanf(argv[2], "%d", &(aux[1])) == 1 && aux[1]>=0 && aux[1]<NRBUF){ //Check args
    if(!list_local(aux[0], aux[1])){ //error when listing
      cyg_mutex_lock(&stdin_mutex);
      printf("No registers to show for the parameters inputed.\n");
      cyg_mutex_unlock(&stdin_mutex);
    }
  } else not_valid();
}

//when user requests for "dr"
void cmd_dr (int argc, char **argv ){
  if(argc==1) delete_local();
  else not_valid();
}

// REQUESTS FOR THE PROCESSING TASK

//when user wants to "cpt"
void cmd_cpt (int argc, char **argv ){
   if(argc==1){
      init_req(req); //malloc
      req->cmd=CODE_CPT;
      cyg_mbox_put(user_pro_channel_H, req); //send request
      //awake event flag - wake the processing task
      cyg_flag_setbits(&ef, efv);
      req_other = (request *) cyg_mbox_get(pro_user_channel_H); //waits for reply
      cyg_mutex_lock(&stdin_mutex);
      printf("Transference Period: %d min", req_other->arg[0]);
      cyg_mutex_unlock(&stdin_mutex);
	free(req_other); //Free
   } else not_valid();
}

//when user requests "mpt"
void cmd_mpt (int argc, char **argv ){
  int aux;
  if(argc==2 && sscanf(argv[1], "%d", &aux)==1 && aux>=0){ //check arguments
    init_req(req); //malloc
    req->cmd=CODE_MPT;
    req->arg[0]=aux; //input arguments
    cyg_mbox_put(user_pro_channel_H, req); //send request
    //awake event flag - wake the processing taks
    cyg_flag_setbits(&ef, efv);
    ack_other = (acknowledge *) cyg_mbox_get(pro_user_channel_H); //waits for answer
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error ? "Not possible to change the transference period": "Transference period changed");
    cyg_mutex_unlock(&stdin_mutex);
    free(ack_other);
  } else not_valid();
}

//when user requests "cttl"
void cmd_cttl (int argc, char **argv ){
  if(argc==1){
    init_req(req); //malloc
    req->cmd=CODE_CTTL;
    cyg_mbox_put(user_pro_channel_H, req); //send request
    //awake event flag - wake the processing taks
    cyg_flag_setbits(&ef, efv);
    req_other = (request *)cyg_mbox_get(pro_user_channel_H); //waits for answer
    cyg_mutex_lock(&stdin_mutex);
    printf("Temperature threshold: %d\nLuminosity threshold: %d\n", req_other->arg[0], req_other->arg[1]);
    cyg_mutex_unlock(&stdin_mutex);
    free(req_other);//free
  } else not_valid();
}

//when user request "dttl"
void cmd_dttl (int argc, char **argv ){
  int aux[2];
  if(argc==3 && sscanf(argv[1], "%d", &(aux[0]))==1 && aux[0]>=0 
             && sscanf(argv[2], "%d", &(aux[1]))==1 && aux[1]>=0 && aux[1]<4){ //check arguments
    init_req(req); //malloc
    req->cmd=CODE_DTTL;
    req->arg[0]=aux[0]; //put the args in the request
    req->arg[1]=aux[1];
    cyg_mbox_put(user_pro_channel_H, req); //Send request
    //awake event flag - wake the processing taks
    cyg_flag_setbits(&ef, efv);
    ack_other = (acknowledge*) cyg_mbox_get(pro_user_channel_H); //waits for reply
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error ? "Not possible to change the thresholds": "Thresholds changed");
    cyg_mutex_unlock(&stdin_mutex);
    free(ack_other);
  } else not_valid();
}

//when user request "pr"
void cmd_pr (int argc, char **argv ){
  int aux[3];
  if(argc==1 || argc==4 || argc==7){
    init_req(req); //malloc
    req->cmd=CODE_PR;
    req->arg[0]=0; //begins with 0 arguments
    if( argc==4 || argc==7){
      if(   sscanf(argv[1], "%d", &(aux[0]))==1 && aux[0]>=0 && aux[0]<24
         && sscanf(argv[2], "%d", &(aux[1]))==1 && aux[1]>=0 && aux[1]<59
         && sscanf(argv[3], "%d", &(aux[2]))==1 && aux[2]>=0 && aux[2]<59){
        req->arg[0]=1; //has 1 timestamps in argument
        req->arg[1]=aux[0];
        req->arg[2]=aux[1];
        req->arg[3]=aux[2];
      } else {
        not_valid();
        return;
      }
      if (argc==7){
        if(   sscanf(argv[4], "%d", &(aux[0]))==1 && aux[0]>=0 && aux[0]<24
           && sscanf(argv[5], "%d", &(aux[1]))==1 && aux[1]>=0 && aux[1]<59
           && sscanf(argv[6], "%d", &(aux[2]))==1 && aux[2]>=0 && aux[2]<59){
        req->arg[4]=aux[0];
        req->arg[5]=aux[1];
        req->arg[6]=aux[2];
        req->arg[0]=2; //has 2 timesstamps in argumetns
        } else {
          not_valid();
          return;
        }
      }
    }
    cyg_mbox_put(user_pro_channel_H, req); //Send request
    //awake event flag - wakes the processing taks
    cyg_flag_setbits(&ef, efv);
    req_other = (request*) cyg_mbox_get(pro_user_channel_H); //waits for reply
    if (req_other->arg[0] == 24){ //error handling
      cyg_mutex_lock(&stdin_mutex);
      printf("Nothing to show as there are no registers in the local memory.");
      cyg_mutex_unlock(&stdin_mutex);
    } else if (req_other->arg[0] == 25 ) { //error handling
      cyg_mutex_lock(&stdin_mutex);
      printf("Nothing to show as there are are no registers in the time interval selected.");
      cyg_mutex_unlock(&stdin_mutex);
    } else { //show results
      cyg_mutex_lock(&stdin_mutex);
      printf("Temperature:\n\tmin: %d\n\tmax: %d\n\tavg: %d\nLuminosity:\n\tmin: %d\n\tmax: %d\n\tavg: %d\n",
            req_other->arg[0], req_other->arg[1], req_other->arg[2], req_other->arg[3], req_other->arg[4], req_other->arg[5]);
      cyg_mutex_unlock(&stdin_mutex);
    }
    free(req_other); //free
  } else not_valid();
}
