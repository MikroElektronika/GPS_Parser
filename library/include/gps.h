#ifndef _GPS_H
#define _GPS_H

#include "gps_parser.h"
#include "gps_defs.h"
#include "time.h"

void gps_init( void );
void gps_put_char( char c );
void gps_process( void );

uint8_t gps_get_current_lon_degrees( void );
double gps_get_current_lon_minutes( void );
AZMUTH_t gps_get_current_lon_azmuth( void );

uint8_t gps_get_current_lat_degrees( void );
double gps_get_current_lat_minutes( void );
AZMUTH_t gps_get_current_lat_azmuth( void );

uint8_t gps_get_current_day( void );
uint8_t gps_get_current_month( void );
uint16_t gps_get_current_year( void );
uint8_t gps_get_current_hour( void );
uint8_t gps_get_current_minute( void );
uint8_t gps_get_current_seconds( void );



#endif