#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <cyg/kernel/kapi.h>

/*-------------------------------------------------------------------------+
| Headers of command functions
+--------------------------------------------------------------------------*/ 
extern void cmd_sair (cyg_uint8, char** );
extern void cmd_rc (cyg_uint8, char** );
extern void cmd_sc (cyg_uint8, char** );
extern void cmd_rtl (cyg_uint8, char** );
extern void cmd_rp (cyg_uint8, char** );
extern void cmd_mmp (cyg_uint8, char** );
extern void cmd_mta (cyg_uint8, char** );
extern void cmd_ra (cyg_uint8, char** );
extern void cmd_dtl (cyg_uint8, char** );
extern void cmd_aa (cyg_uint8, char** );
extern void cmd_ir (cyg_uint8, char** );
extern void cmd_trc (cyg_uint8, char** );
extern void cmd_tri (cyg_uint8, char** );
extern void cmd_irl (cyg_uint8, char** );
extern void cmd_lr (cyg_uint8, char** );
extern void cmd_dr (cyg_uint8, char** );
extern void cmd_cpt (cyg_uint8, char** );
extern void cmd_mpt (cyg_uint8, char** );
extern void cmd_cttl (cyg_uint8, char** );
extern void cmd_dttl (cyg_uint8, char** );
extern void cmd_pr (cyg_uint8, char** );
//this file functions
       void cmd_sos  (cyg_uint8, char** );
       void cmd_ini (cyg_uint8, char** );
       void monitor(void);
       //void dbg(char []);

//sync stuff
extern cyg_mutex_t stdin_mutex;

/*-------------------------------------------------------------------------+
| Variable and constants definitin
+--------------------------------------------------------------------------*/ 
const char TitleMsg[] = "\nWeather Station  - Remote Console (2019/20)\n\tBy: Pedro, Sofia & Vasco\n";
const char InvalMsg[] = "\nInvalid command!";

struct 	command_d {
  void  (*cmd_fnct)(cyg_uint8, char**);
  char*	cmd_name;
  char*	cmd_help;
} const commands[] = {
  {cmd_sos,   "sos",  "\n\t help"},
  {cmd_sair,  "sair", "\n\t sair"},
  {cmd_ini,   "ini",  "<d>\n\t inicializar dispositivo (0/1) ser0/ser1"},
  {cmd_rc,    "rc",   "<d>\n\t read clock"},
  {cmd_sc,    "sc",   "<h m s>\n\t set clock"},
  {cmd_rtl,   "rtl",  "\n\t read temperature and luminosity"},
  {cmd_rp,    "rp",   "\n\t read parameter (PMON, TALA)"},
  {cmd_mmp,   "mmp",  "<p>\n\t modify monitoring period (seconds -0 deactivate)"},
  {cmd_mta,   "mta",  "<t>\n\t modify time alarm (seconds)"},
  {cmd_ra,    "ra",   "\n\t read alarms (temperature, luminosity, active/inactive-1/0"},
  {cmd_dtl,   "dtl",  "<t l>\n\t define alarm temperature and luminosity"},
  {cmd_aa,    "aa",   "<a>\n\t activate/deactivate alarms (1/0)"},
  {cmd_ir,    "ir",   "\n\t information about registers (NREG, nr, iread, iwrite)"},
  {cmd_trc,   "trc",  "<n>\n\t transfer n registers from current iread position"},
  {cmd_tri,   "tri",  "<n i>\n\t transfer n registers from index i (0 - oldest)"},
  {cmd_irl,   "irl",  "\n\t information about local registers (NRBUF, nr, iread, iwrite)"},
  {cmd_lr,    "lr",   "<n i>\n\t list n registers (local memory) from index i (0 - oldest)"},
  {cmd_dr,    "dr",   "\n\t delete registers (local memory)"},
  {cmd_cpt,   "cpt",  "\n\t check period of transference"},
  {cmd_mpt,   "mpt",  "<p>\n\t modify period of transference (minutes - 0 deactivate)"},
  {cmd_cttl,  "cttl", "\n\t check threshold temperature luminosity for processing"},
  {cmd_dttl,  "dttl", "<t l>\n\t define threshold temperature and luminosity for processing"},
  {cmd_pr,  "pr", "<[h1 m1 s1 [h2 m2 s2]]\n\t process registers (max, min, mean) between instants t1 and t2 (h, m ,s)"}
};

#define NCOMMANDS  (sizeof(commands)/sizeof(struct command_d))
#define ARGVECSIZE 10
#define MAX_LINE   50

/*-------------------------------------------------------------------------+
| Function: cmd_sos - provides a rudimentary help
+--------------------------------------------------------------------------*/ 
void cmd_sos (cyg_uint8 argc, char **argv){
  cyg_uint8 i;
  cyg_mutex_lock(&stdin_mutex);
  printf("%s\n", TitleMsg);
  for (i=0; i<NCOMMANDS; i++)
    printf("%s %s\n", commands[i].cmd_name, commands[i].cmd_help);
  cyg_mutex_unlock(&stdin_mutex);
}

/*-------------------------------------------------------------------------+
| Function: getline        (called from monitor) 
+--------------------------------------------------------------------------*/ 
cyg_uint8 my_getline (char** argv, cyg_uint8 argvsize){
  static char line[MAX_LINE];
  char *p;
  cyg_uint8 argc;

  fgets(line, MAX_LINE, stdin); //o que fazer a isto TODO ?? devo proteger ou nao ?

  /* Break command line into an o.s. like argument vector,
     i.e. compliant with the (cyg_uint8 argc, char **argv) specification --------*/
  for (argc=0,p=line; (*line != '\0') && (argc < argvsize); p=NULL,argc++){
    p = strtok(p, " \t\n");
    argv[argc] = p;
    if (p == NULL) return argc;
  }
  argv[argc] = p;
  return argc;
}

/*-------------------------------------------------------------------------+
| Function: monitor        (called from main) 
+--------------------------------------------------------------------------*/ 
void monitor (void){
  static char *argv[ARGVECSIZE+1], *p;
  cyg_uint8 argc, i;
  

  cyg_mutex_lock(&stdin_mutex);
  printf("%s Type sos for help\n", TitleMsg);
  cyg_mutex_unlock(&stdin_mutex);
  for (;;) {
    cyg_mutex_lock(&stdin_mutex);
    printf("\nCmd> ");
    cyg_mutex_unlock(&stdin_mutex);
    /* Reading and parsing command line  ----------------------------------*/
    if ((argc = my_getline(argv, ARGVECSIZE)) > 0) {
      for (p=argv[0]; *p != '\0'; *p=tolower(*p), p++);
      for (i = 0; i < NCOMMANDS; i++) 
        if (strcmp(argv[0], commands[i].cmd_name) == 0) 
	        break;
      /* Executing commands -----------------------------------------------*/
      if (i < NCOMMANDS)
        commands[i].cmd_fnct (argc, argv);
      else{
        cyg_mutex_lock(&stdin_mutex);
        printf("%s", InvalMsg);
        cyg_mutex_unlock(&stdin_mutex);
      }
    } /* if my_getline */
  } /* forever */
}
