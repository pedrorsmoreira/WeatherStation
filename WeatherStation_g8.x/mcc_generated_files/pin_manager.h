/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC16F18875
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB 	          :  MPLAB X 5.20	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set L0 aliases
#define L0_TRIS                 TRISAbits.TRISA4
#define L0_LAT                  LATAbits.LATA4
#define L0_PORT                 PORTAbits.RA4
#define L0_WPU                  WPUAbits.WPUA4
#define L0_OD                   ODCONAbits.ODCA4
#define L0_ANS                  ANSELAbits.ANSA4
#define L0_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define L0_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define L0_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define L0_GetValue()           PORTAbits.RA4
#define L0_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define L0_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define L0_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define L0_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define L0_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define L0_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define L0_SetAnalogMode()      do { ANSELAbits.ANSA4 = 1; } while(0)
#define L0_SetDigitalMode()     do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set L1 aliases
#define L1_TRIS                 TRISAbits.TRISA5
#define L1_LAT                  LATAbits.LATA5
#define L1_PORT                 PORTAbits.RA5
#define L1_WPU                  WPUAbits.WPUA5
#define L1_OD                   ODCONAbits.ODCA5
#define L1_ANS                  ANSELAbits.ANSA5
#define L1_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define L1_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define L1_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define L1_GetValue()           PORTAbits.RA5
#define L1_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define L1_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define L1_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define L1_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define L1_SetPushPull()        do { ODCONAbits.ODCA5 = 0; } while(0)
#define L1_SetOpenDrain()       do { ODCONAbits.ODCA5 = 1; } while(0)
#define L1_SetAnalogMode()      do { ANSELAbits.ANSA5 = 1; } while(0)
#define L1_SetDigitalMode()     do { ANSELAbits.ANSA5 = 0; } while(0)

// get/set A aliases
#define A_TRIS                 TRISAbits.TRISA6
#define A_LAT                  LATAbits.LATA6
#define A_PORT                 PORTAbits.RA6
#define A_WPU                  WPUAbits.WPUA6
#define A_OD                   ODCONAbits.ODCA6
#define A_ANS                  ANSELAbits.ANSA6
#define A_SetHigh()            do { LATAbits.LATA6 = 1; } while(0)
#define A_SetLow()             do { LATAbits.LATA6 = 0; } while(0)
#define A_Toggle()             do { LATAbits.LATA6 = ~LATAbits.LATA6; } while(0)
#define A_GetValue()           PORTAbits.RA6
#define A_SetDigitalInput()    do { TRISAbits.TRISA6 = 1; } while(0)
#define A_SetDigitalOutput()   do { TRISAbits.TRISA6 = 0; } while(0)
#define A_SetPullup()          do { WPUAbits.WPUA6 = 1; } while(0)
#define A_ResetPullup()        do { WPUAbits.WPUA6 = 0; } while(0)
#define A_SetPushPull()        do { ODCONAbits.ODCA6 = 0; } while(0)
#define A_SetOpenDrain()       do { ODCONAbits.ODCA6 = 1; } while(0)
#define A_SetAnalogMode()      do { ANSELAbits.ANSA6 = 1; } while(0)
#define A_SetDigitalMode()     do { ANSELAbits.ANSA6 = 0; } while(0)

// get/set C aliases
#define C_TRIS                 TRISAbits.TRISA7
#define C_LAT                  LATAbits.LATA7
#define C_PORT                 PORTAbits.RA7
#define C_WPU                  WPUAbits.WPUA7
#define C_OD                   ODCONAbits.ODCA7
#define C_ANS                  ANSELAbits.ANSA7
#define C_SetHigh()            do { LATAbits.LATA7 = 1; } while(0)
#define C_SetLow()             do { LATAbits.LATA7 = 0; } while(0)
#define C_Toggle()             do { LATAbits.LATA7 = ~LATAbits.LATA7; } while(0)
#define C_GetValue()           PORTAbits.RA7
#define C_SetDigitalInput()    do { TRISAbits.TRISA7 = 1; } while(0)
#define C_SetDigitalOutput()   do { TRISAbits.TRISA7 = 0; } while(0)
#define C_SetPullup()          do { WPUAbits.WPUA7 = 1; } while(0)
#define C_ResetPullup()        do { WPUAbits.WPUA7 = 0; } while(0)
#define C_SetPushPull()        do { ODCONAbits.ODCA7 = 0; } while(0)
#define C_SetOpenDrain()       do { ODCONAbits.ODCA7 = 1; } while(0)
#define C_SetAnalogMode()      do { ANSELAbits.ANSA7 = 1; } while(0)
#define C_SetDigitalMode()     do { ANSELAbits.ANSA7 = 0; } while(0)

// get/set IO_RB4 aliases
#define IO_RB4_TRIS                 TRISBbits.TRISB4
#define IO_RB4_LAT                  LATBbits.LATB4
#define IO_RB4_PORT                 PORTBbits.RB4
#define IO_RB4_WPU                  WPUBbits.WPUB4
#define IO_RB4_OD                   ODCONBbits.ODCB4
#define IO_RB4_ANS                  ANSELBbits.ANSB4
#define IO_RB4_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define IO_RB4_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define IO_RB4_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define IO_RB4_GetValue()           PORTBbits.RB4
#define IO_RB4_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define IO_RB4_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define IO_RB4_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define IO_RB4_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define IO_RB4_SetPushPull()        do { ODCONBbits.ODCB4 = 0; } while(0)
#define IO_RB4_SetOpenDrain()       do { ODCONBbits.ODCB4 = 1; } while(0)
#define IO_RB4_SetAnalogMode()      do { ANSELBbits.ANSB4 = 1; } while(0)
#define IO_RB4_SetDigitalMode()     do { ANSELBbits.ANSB4 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/