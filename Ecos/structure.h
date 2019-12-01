#include <stdio.h>
#include <stdlib.h>
#include <cyg/kernel/kapi.h>

#define REQUEST_SIZE 10
#define CMD_LEN 5
#define N_ARGS 7
#define NRBUF 100
#define NREG 20 //TODO: For debugging only

//Request codes
#define CODE_EXIT -1
#define CODE_RC 1
#define CODE_SC 2
#define CODE_RTL 3
#define CODE_RP 4
#define CODE_MMP 5
#define CODE_MTA 6
#define CODE_RA 7
#define CODE_DTL 8
#define CODE_AA 9
#define CODE_IR 10
#define CODE_TRC 11
#define CODE_TRI 12
#define CODE_IRL 13
#define CODE_LR 14
#define CODE_DR 15
#define CODE_CPT 16
#define CODE_MPT 17
#define CODE_CTTL 18
#define CODE_DTTL 19
#define CODE_PR 20

//Request message struct
typedef struct Request {
    int cmd;
    int arg[N_ARGS];
} request;

//Acknowledge message struct
typedef struct Acknowledge {
    bool error;
} acknowledge;

//Register message struct
typedef struct Buffer {
    int temperature;
    int luminosity;
    int hour;
    int minute;
    int second;
} buffer ;

//Local memory struct
typedef struct Local {
    int nr;
    int iread;
    int iwrite; //head
    buffer reg[NRBUF];
} local;

//Pic memory struct
typedef struct PicMemory{
    int nr;
    int iread;
    int iwrite;
    buffer reg[NREG];
} pic_memory;



