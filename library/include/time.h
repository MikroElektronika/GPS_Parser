/****************************************************************************
* Title                 :   Time library
* Filename              :   time.h
* Author                :   RL
* Origin Date           :   08/25/2015
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date    Software Version    Initials   Description
*  08/25/15    1.01              RL         Platform independent version
*
*****************************************************************************/
/** @file time.h
 *  @brief Time library for MIkroC
 *
 *  @date 25 Aug 2015
 *  @author Richard Lowe
 *  @copyright GNU Public License
 *
 *  @version .1 - Initial testing and verification
 *
 *  @note Test configuration:
 *   MCU:             STM32F107VC
 *   Dev.Board:       EasyMx Pro v7
 *   Oscillator:      72 Mhz internal
 *   Ext. Modules:    GPS Click
 *   SW:              ARM 4.5.2
 *
 */
#ifndef _TIME_H
#define _TIME_H

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>

/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
/**
 * Some constants
 */
#define Time_secInMn    60                      // seconds per minute
#define Time_secInH     (Time_secInMn * 60)     // seconds per hour
#define Time_secIn24h   (Time_secInH * 24)      // seconds per day


/******************************************************************************
* Configuration Constants
*******************************************************************************/


/******************************************************************************
* Macros
*******************************************************************************/



/******************************************************************************
* Typedefs
*******************************************************************************/
typedef struct
{
    unsigned char   ss ;    // seconds
    unsigned char   mn ;    // minutes
    unsigned char   hh ;    // hours
    unsigned char   md ;    // day in month, from 1 to 31
    unsigned char   wd ;    // day in week, monday=0, tuesday=1, .... sunday=6
    unsigned char   mo ;    // month number, from 1 to 12 (and not from 0 to 11 as with unix C time !)
    unsigned int    yy ;    // year Y2K compliant, from 1892 to 2038
} TimeStruct;


/******************************************************************************
* Variables
*******************************************************************************/
extern  long            Time_jd1970 ;   // 01/01/1970 julian day number

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*
 * public functions
 */
long Time_dateToEpoch(TimeStruct *ts) ;
long Time_dateDiff(TimeStruct *t1, TimeStruct *t2);
void Time_epochToDate(long e, TimeStruct *ts) ;


#ifdef __cplusplus
} // extern "C"
#endif

#endif /*TIME_H_*/

/*** End of File **************************************************************/

