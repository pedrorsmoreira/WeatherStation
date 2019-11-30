/* 
 * File:   communications.h
 * Author: SCE
 *
 * Created on November 25, 2019, 11:41 AM
 */

#ifndef COMMUNICATIONS_H
#define	COMMUNICATIONS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/* It is assumed that SOM and EOM values do not occur in the message */
#define SOM 0xFD /* start of message */
#define EOM 0xFE /* end of message */
    
#define RCLK 0xC0 /* read clock */
#define SCLK 0XC1 /* set clock */
#define RTL 0XC2 /* read temperature and luminosity */
#define RPAR 0XC3 /* read parameters */
#define MMP 0XC4 /* modify monitoring period */
#define MTA 0XC5 /* modify time alarm */
#define RALA 0XC6 /* read alarms (temperature, luminosity, active/inactive) */
#define DATL 0XC7 /* define alarm temperature and luminosity */
#define AALA 0XC8 /* activate/deactivate alarms */
#define IREG 0XC9 /* information about registers (NREG, nr, iread, iwrite)*/
#define TRGC 0XCA /* transfer registers (curr. position)*/
#define TRGI 0XCB /* transfer registers (index) */
#define NMFL 0XCC /* notification memory (half) full */
    
#define CMD_OK 0 /* command successful */
#define CMD_ERROR 0xFF /* error in command */
    

void read_msgs(void);
void interpret_message(char *);
void read_clock(void);
void set_clock(char *);
void read_temp_lum(void);
void read_param(void);
void modify_pmon(char *);
void modify_time_alarm(char *);
void read_alarms(void);
void define_alarm(char *);
void switch_alarm(char *);
void registers_info(void);
void transfer_n_registers(char *);
void transfer_registers_i(char *);
bool transfer_registers(uint8_t, uint8_t);
void notification_memory(void);
void send_msg(char *);
void send_confirmation(uint8_t, uint8_t);
void countMsg(void);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMUNICATIONS_H */

