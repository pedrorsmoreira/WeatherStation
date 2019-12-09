#include <cyg/io/io.h>
#include "structure.h"

extern cyg_handle_t com_user_channel_H;
extern cyg_handle_t user_com_channel_H;
extern cyg_handle_t pro_user_channel_H;
extern cyg_handle_t user_pro_channel_H;
extern cyg_mutex_t stdin_mutex;
extern cyg_io_handle_t serH;

static Cyg_ErrNo err;
static request *req = NULL;
static request *req_other = NULL;
static acknowledge *ack_other = NULL;
static acknowledge *ack = NULL;
static buffer *reg_other = NULL;

extern void init_req(request*);
extern void init_ack(acknowledge*);
extern void info_local(void);
extern bool list_local(int, int);
extern void delete_local(void);

/*-------------------------------------------------------------------------+
| Function: cmd_ini - inicializar dispositivo
+--------------------------------------------------------------------------*/ 
void cmd_ini(int argc, char **argv){
  //printf("io_lookup\n");
  if ((argc > 1) && (argv[1][0] = '1'))
    err = cyg_io_lookup("/dev/ser1", &serH);
  else err = cyg_io_lookup("/dev/ser0", &serH);
  //printf("lookup err=%x\n", err);
  
  //init das variáveis das perguntas e respostas
  //req=(request*)malloc(sizeof(request));
  ack=(acknowledge*)malloc(sizeof(acknowledge));
  init_req(req);
  init_ack(ack);
}
/*-------------------------------------------------------------------------+
| Function: cmd_sair - termina a aplicacao
+--------------------------------------------------------------------------*/ 
//void printnothing(void){
//  cyg_mutex_lock(&stdin_mutex);
//  printf("Nothing here yet.\n");
//  cyg_mutex_unlock(&stdin_mutex);
//}

void not_valid(void){
  cyg_mutex_lock(&stdin_mutex);
  printf("Command not valid.\n");
  cyg_mutex_unlock(&stdin_mutex);
}

void cmd_sair (int argc, char **argv){
  init_req(req);
  req->cmd=CODE_EXIT;
  cyg_mbox_put(user_com_channel_H, req); //envia request para a outra thread se desligar
  free(req); //Desliga-se
  free(ack);
  cyg_mutex_lock(&stdin_mutex);
  printf("Exiting\n.");
  cyg_mutex_unlock(&stdin_mutex);
  cyg_thread_exit();
}

void cmd_rc ( int argc, char **argv) { //TODO: Por unidades nisto tudo 
  if (argc==1){
//printf("Valid\n");
    init_req(req);
    req->cmd=CODE_RC;
//printf("Put\n");
    cyg_mbox_put(user_com_channel_H, req); //envia o request
//printf("After putting\n");
    req_other = (request *) cyg_mbox_get(com_user_channel_H); //recebe a resposta
//printf("After get request\n");
    cyg_mutex_lock(&stdin_mutex);
    printf("clock: %u:%u:%u\n", req_other->arg[0], req_other->arg[1], req_other->arg[2]);
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_sc (int argc, char **argv ){
  int aux[3], i;
  bool flag = true;
  if ( argc == 4 ){ //check if 3 arguments (h,m,s) TODO: Fazer um mega if se calhar
    for (i=1; i<4; i++){
      if (sscanf( argv[i], "%d", &(aux[i-1])) != 1 || aux[i-1]<0){
        flag = false;
        break;
      }
    }
  } else flag = false;
  if (flag){
    if ( aux[0] < 23 && aux[1] < 59 && aux[2] < 59){ //check time integrity
      init_req(req);
      req->cmd=CODE_SC;
      for (i=0; i<3; i++) req->arg[i]=aux[i]; //put the time in the arguments
      cyg_mbox_put(user_com_channel_H, req); //enviar o comando
      ack_other = (acknowledge *) cyg_mbox_get(com_user_channel_H); // obter o ok 
      cyg_mutex_lock(&stdin_mutex);
      printf("%s.\n", ack_other->error ? "Not possible to change the clock" : "Clock changed");
      cyg_mutex_unlock(&stdin_mutex);
    } else not_valid();
  } else not_valid();
}
void cmd_rtl (int argc, char **argv ){
  if (argc==1){
    init_req(req);
    req->cmd=CODE_RTL;
    cyg_mbox_put(user_com_channel_H, req); //envia o request
    req_other = (request *) cyg_mbox_get(com_user_channel_H); //recebe a resposta
    cyg_mutex_lock(&stdin_mutex);
    printf("Temperature: %d\nLuminosity: %d\n", req_other->arg[0], req_other->arg[1]);
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_rp (int argc, char **argv ){
   if (argc==1){
    init_req(req);
    req->cmd=CODE_RP;
    cyg_mbox_put(user_com_channel_H, req); //envia o request
    req_other = (request *) cyg_mbox_get(com_user_channel_H); //recebe a resposta
    cyg_mutex_lock(&stdin_mutex);
    printf("PMON: %d\nTALA: %d\n", req_other->arg[0], req_other->arg[1]);
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_mmp (int argc, char **argv ){
  int aux;
  if ( argc == 2 && sscanf( argv[1], "%d", &(aux)) == 1 && aux>=0){
    init_req(req);
    req->cmd=CODE_MMP;
    req->arg[0]=aux; //put the period in the arguments
    cyg_mbox_put(user_com_channel_H, req); //enviar o comando
    ack_other = (acknowledge *) cyg_mbox_get(com_user_channel_H); // obter o ok 
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error? "Not possible to change the monitoring period": "Monitoring period changed");
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_mta (int argc, char **argv ){
  int aux;
  if ( argc == 2 && sscanf( argv[1], "%d", &(aux)) == 1 && aux>=0){//TODO: verificar se o valor pode ser 0 ?
    init_req(req);
    req->cmd=CODE_MTA;
    req->arg[0]=aux; //put the time alarm in the arguments
    cyg_mbox_put(user_com_channel_H, req); //enviar o comando
    ack_other = (acknowledge *) cyg_mbox_get(com_user_channel_H); // obter o ok 
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error? "Not possible to change the time alarm": "Time alarm changed");
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}

void cmd_ra (int argc, char **argv ){
    if (argc==1){
    init_req(req);
    req->cmd=CODE_RA;
    cyg_mbox_put(user_com_channel_H, req); //envia o request
    req_other = (request *) cyg_mbox_get(com_user_channel_H); //recebe a resposta
    cyg_mutex_lock(&stdin_mutex);
    printf("Temperature Alarm: %d\nLuminosity Alarm: %d\nActive? %s\n", 
      req_other->arg[0], req_other->arg[1],  req_other->arg[2]? "yes":"no" );
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_dtl (int argc, char **argv ){
  int aux[2], i;
  bool flag = true;
  if ( argc == 3 ){ //check if 2 arguments (t and l) TODO: Fazer um mega if probably
    for (i=1; i<3; i++){
      if (sscanf( argv[i], "%d", &(aux[i-1])) != 1 || aux[i-1]<0){
        flag = false;
        break;
      }
    }
  } else flag = false;
  if (flag){
    init_req(req);
    req->cmd=CODE_DTL;
    for (i=0; i<2; i++) req->arg[i]=aux[i]; //put the alarms in the arguments
    cyg_mbox_put(user_com_channel_H, req); //enviar o comando
    ack_other = (acknowledge *) cyg_mbox_get(com_user_channel_H); // obter o ok 
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error ? "Not possible to change the parameters" : "Alarms changed");
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_aa (int argc, char **argv ){
  int aux;
  if ( argc == 2 && sscanf( argv[1], "%d", &(aux)) == 1 && (aux==0 || aux==1)){
    init_req(req);
    req->cmd=CODE_AA;
    req->arg[0]=aux; //put the alarm state in the arguments
    cyg_mbox_put(user_com_channel_H, req); //enviar o comando
    ack_other = (acknowledge *) cyg_mbox_get(com_user_channel_H); // obter o ok
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error? "Not possible to change the alarm state": "Alarm state changed");
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_ir (int argc, char **argv ){
  if (argc==1){
    init_req(req);
    req->cmd=CODE_IR;
    cyg_mbox_put(user_com_channel_H, req); //envia o request
    req_other = (request *) cyg_mbox_get(com_user_channel_H); //recebe a resposta
    cyg_mutex_lock(&stdin_mutex);
    printf("NREG: %d\nnr: %d\niread: %d\niwrite: %d\n", req_other->arg[0], req_other->arg[1], req_other->arg[2], req_other->arg[3]);
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_trc (int argc, char **argv ){
  int aux, i=1;
  if (argc ==2 && sscanf(argv[1], "%d", &(aux)) == 1 && aux>0 && aux<11){ //n max is 10 (enunciado)
    init_req(req);
    req->cmd=CODE_TRC;
    req->arg[0]=aux; //por o numero de registos nos argumentos
    cyg_mbox_put(user_com_channel_H, req);
printf("HEY\n");
      ack_other = (acknowledge *) cyg_mbox_get(com_user_channel_H); // obter o ok 
      cyg_mutex_lock(&stdin_mutex);
      printf("%s.\n", ack_other->error ? "Not possible to change the clock" : "Clock changed");
      cyg_mutex_unlock(&stdin_mutex);
	return;
    cyg_mutex_lock(&stdin_mutex);
    printf("Registers:\n");
    cyg_mutex_unlock(&stdin_mutex);
    ack->error=false;
    while (1){
      reg_other = (buffer *) cyg_mbox_get(com_user_channel_H);
      if(reg_other->hour!=-1){
      cyg_mutex_lock(&stdin_mutex);
      printf("%d - [%d:%d:%d] t=%d, l=%d\n", 
            i, reg_other->hour, reg_other->minute, reg_other->second, reg_other->temperature, reg_other->luminosity);
      cyg_mutex_unlock(&stdin_mutex);
      cyg_mbox_put(user_com_channel_H, ack);
      i++;
      } else break;
    }
    if (i==1) {
      cyg_mutex_lock(&stdin_mutex);
      printf("No registers to show.\n");
      cyg_mutex_unlock(&stdin_mutex);
    }
  } else not_valid();
}
void cmd_tri (int argc, char **argv ){
  int aux[2], i=0;
  if (argc ==3 && sscanf(argv[1], "%d", &(aux[0])) == 1 && aux[0]>0 && aux[0]<11
               && sscanf(argv[2], "%d", &(aux[1])) == 1 && aux[1]>=0 && aux[1]<NREG){ //n max is 10 (enunciado) e i max é NREG
    init_req(req);
    req->cmd=CODE_TRI;
    req->arg[0]=aux[0]; //por o numero de registos nos argumentos
    req->arg[1]=aux[1]; //por o indice do primeiro registo nos argumentos
    cyg_mbox_put(user_com_channel_H, req);
      ack_other = (acknowledge *) cyg_mbox_get(com_user_channel_H); // obter o ok 
      cyg_mutex_lock(&stdin_mutex);
      printf("%s.\n", ack_other->error ? "Not possible to change the clock" : "Clock changed");
      cyg_mutex_unlock(&stdin_mutex);
	return;
    //printf("n: %d, i: %d", req->arg[0], req->arg[1]);
    printf("Registers:\n");
    ack->error=false;
    while (1){
      reg_other = (buffer *) cyg_mbox_get(com_user_channel_H);
      if (reg_other->hour!=-1){
        cyg_mutex_lock(&stdin_mutex);
        printf("%d - [%d:%d:%d] t=%d, l=%d\n", 
              aux[1]+i, reg_other->hour, reg_other->minute, reg_other->second, reg_other->temperature, reg_other->luminosity);
        cyg_mutex_unlock(&stdin_mutex);
        cyg_mbox_put(user_com_channel_H, ack);
        i++;
      } else break;
    }
    if (i==0) not_valid();
  } else not_valid();
}

// COMANDOS DIRETOS À MEMORIA LOCAL
void cmd_irl (int argc, char **argv ){
  if (argc==1) info_local();
  else not_valid();
}

void cmd_lr (int argc, char **argv ){
  int aux[2];
  if (argc ==3 && sscanf(argv[1], "%d", &(aux[0])) == 1 && aux[0]>0 && aux[0]<NRBUF
               && sscanf(argv[2], "%d", &(aux[1])) == 1 && aux[1]>=0 && aux[1]<NRBUF){
    if(!list_local(aux[0], aux[1])){
      cyg_mutex_lock(&stdin_mutex);
      printf("No registers to show for the parameters inputed.\n");
      cyg_mutex_unlock(&stdin_mutex);
    }
  } else not_valid();
}
void cmd_dr (int argc, char **argv ){
  if(argc==1) delete_local();
  else not_valid();
}

// COMANDOS AO PROCESSADOR 
void cmd_cpt (int argc, char **argv ){
   if(argc==1){
      init_req(req);
      req->cmd=CODE_CPT;
      cyg_mbox_put(user_pro_channel_H, req);
      req_other = (request *) cyg_mbox_get(pro_user_channel_H);
      cyg_mutex_lock(&stdin_mutex);
      printf("Transference Period: %d min", req_other->arg[0]);
      cyg_mutex_unlock(&stdin_mutex);
   } else not_valid();
}
void cmd_mpt (int argc, char **argv ){
  int aux;
  if(argc==2 && sscanf(argv[1], "%d", &aux)==1 && aux>=0){
    init_req(req);
    req->cmd=CODE_MPT;
    req->arg[0]=aux;
    cyg_mbox_put(user_pro_channel_H, req);
    ack_other = (acknowledge *) cyg_mbox_get(pro_user_channel_H);
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error ? "Not possible to change the transference period": "Transference period changed");
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_cttl (int argc, char **argv ){
  if(argc==1){
    init_req(req);
    req->cmd=CODE_CTTL;
    cyg_mbox_put(user_pro_channel_H, req);
    req_other = (request *)cyg_mbox_get(pro_user_channel_H);
    cyg_mutex_lock(&stdin_mutex);
    printf("Temperature threshold: %d\nLuminosity threshold: %d\n", req_other->arg[0], req_other->arg[1]);
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}
void cmd_dttl (int argc, char **argv ){
  int aux[2];
  if(argc==3 && sscanf(argv[1], "%d", &(aux[0]))==1 && aux[0]>=0 
             && sscanf(argv[2], "%d", &(aux[1]))==1 && aux[1]>=0 && aux[1]<4){
    init_req(req);
    req->cmd=CODE_DTTL;
    req->arg[0]=aux[0];
    req->arg[1]=aux[1];
    cyg_mbox_put(user_pro_channel_H, req);
    ack_other = (acknowledge*) cyg_mbox_get(pro_user_channel_H);
    cyg_mutex_lock(&stdin_mutex);
    printf("%s.\n", ack_other->error ? "Not possible to change the thresholds": "Thresholds changed");
    cyg_mutex_unlock(&stdin_mutex);
  } else not_valid();
}

void cmd_pr (int argc, char **argv ){
  int aux[3];
  if(argc==1 || argc==4 || argc==7){
    init_req(req);
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
    cyg_mbox_put(user_pro_channel_H, req);
    req_other = (request*) cyg_mbox_get(pro_user_channel_H);
    if (req_other->arg[0] == -1){
      cyg_mutex_lock(&stdin_mutex);
      printf("Nothing to show as there are no registers in the local memory.");
      cyg_mutex_unlock(&stdin_mutex);
    } else if (req_other->arg[0] == -2 ) {
      cyg_mutex_lock(&stdin_mutex);
      printf("Nothing to show as there are are no registers in the time interval selected.");
      cyg_mutex_unlock(&stdin_mutex);
    } else {
      cyg_mutex_lock(&stdin_mutex);
      printf("Temperature:\n\tmin: %d\n\tmax: %d\n\tavg: %d\nLuminosity:\n\tmin: %d\n\tmax: %d\n\tavg: %d\n",
            req_other->arg[0], req_other->arg[1], req_other->arg[2], req_other->arg[3], req_other->arg[4], req_other->arg[5]);
      cyg_mutex_unlock(&stdin_mutex);
    }
  } else not_valid();
}



/*-------------------------------------------------------------------------+
| Function: cmd_test - apenas como exemplo
+-------------------------------------------------------------------------- 
void cmd_test (int argc, char** argv)
{
  int i;

  // exemplo -- escreve argumentos 
  for (i=0; i<argc; i++)
    printf ("\nargv[%d] = %s", i, argv[i]);
}

/-------------------------------------------------------------------------+
| Function: cmd_ems - enviar mensagem (string)
+-------------------------------------------------------------------------- 
void cmd_ems (int argc, char **argv)
{
  unsigned int n;

  if (argc > 1) {
    n = strlen(argv[1]) + 1;
    err = cyg_io_write(serH, argv[1], &n);
    printf("io_write err=%x, n=%d str=%s\n", err, n, argv[1]);
  }
  else {
    n = 10;
    err = cyg_io_write(serH, "123456789", &n);
    printf("io_write err=%x, n=%d str=%s\n", err, n, "123456789");
  }
}

/-------------------------------------------------------------------------+
| Function: cmd_emh - enviar mensagem (hexadecimal)
+-------------------------------------------------------------------------- 
void cmd_emh (int argc, char **argv)
{
  unsigned int n, i;
  unsigned char bufw[50];

  if ((n=argc) > 1) {
    n--;
    if (n > 50) n = 50;
    for (i=0; i<n; i++)
      //      sscanf(argv[i+1], "%hhx", &bufw[i]);
      {unsigned int x; sscanf(argv[i+1], "%x", &x); bufw[i]=(unsigned char)x;}
    err = cyg_io_write(serH, bufw, &n);
    printf("io_write err=%x, n=%d\n", err, n);
    for (i=0; i<n; i++)
      printf("buf[%d]=%x\n", i, bufw[i]);
  }
  else {
    printf("nenhuma mensagem!!!\n");
  }
}

/-------------------------------------------------------------------------+
| Function: cmd_rms - receber mensagem (string)
+-------------------------------------------------------------------------- 
void cmd_rms (int argc, char **argv)
{
  unsigned int n;
  char bufr[50];

  if (argc > 1) n = atoi(argv[1]);
  if (n > 50) n = 50;
  err = cyg_io_read(serH, bufr, &n);
  printf("io_read err=%x, n=%d buf=%s\n", err, n, bufr);
}

/-------------------------------------------------------------------------+
| Function: cmd_rmh - receber mensagem (hexadecimal)
+-------------------------------------------------------------------------- 
void cmd_rmh (int argc, char **argv)
{
  unsigned int n, i;
  unsigned char bufr[50];

  if (argc > 1) n = atoi(argv[1]);
  if (n > 50) n = 50;
  err = cyg_io_read(serH, bufr, &n);
  printf("io_read err=%x, n=%d\n", err, n);
  for (i=0; i<n; i++)
    printf("buf[%d]=%x\n", i, bufr[i]);
}

*/


