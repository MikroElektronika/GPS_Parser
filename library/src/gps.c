#include "gps.h"
#include "gps_config.h"

void gps_init( void )
{

}

void gps_put_char( char c )
{
    gps_put( c );
}

void gps_process( void )
{
    gps_parse();
}

uint8_t gps_get_current_lon_degrees()
{
    location_t* tmp_lon = gps_get_current_lon();

    return tmp_lon->degrees;
}

double gps_get_current_lon_minutes()
{
    location_t* tmp_lon = gps_get_current_lon();

    return tmp_lon->minutes;
}

AZMUTH_t gps_get_current_lon_azmuth()
{
    location_t* tmp_lon = gps_get_current_lon();

    return tmp_lon->azmuth;
}

uint8_t gps_get_current_lat_degrees()
{
    location_t* tmp_lat = gps_get_current_lat();

    return tmp_lat->degrees;
}

double gps_get_current_lat_minutes()
{
    location_t* tmp_lat = gps_get_current_lat();

    return tmp_lat->minutes;
}

AZMUTH_t gps_get_current_lat_azmuth()
{
    location_t* tmp_lat = gps_get_current_lat();

    return tmp_lat->azmuth;
}

uint8_t gps_get_current_day()
{
    TimeStruct *ts = gps_get_current_time();
    return ts->md;
}

uint8_t gps_get_current_month()
{
    TimeStruct *ts = gps_get_current_time();
    return ts->mo;
}

uint16_t gps_get_current_year()
{
    TimeStruct *ts = gps_get_current_time();
    return ts->yy;
}

uint8_t gps_get_current_hour()
{
    TimeStruct *ts = gps_get_current_time();
    return ts->hh;
}

uint8_t gps_get_current_minute()
{
    TimeStruct *ts = gps_get_current_time();
    return ts->mn;
}

uint8_t gps_get_current_seconds()
{
    TimeStruct *ts = gps_get_current_time();
    return ts->ss;
}