#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include "gps_defs.h"
#include "gps_parser.h"

#ifdef  __MIKROC_PRO_FOR_AVR__
#elif __MIKROC_PRO_FOR_PIC__
#elif __MIKROC_PRO_FOR_DSPIC__
#elif __MIKROC_PRO_FOR_PIC32__
#elif __MIKROC_PRO_FOR_8051__
#elif __MIKROC_PRO_FOR_FT90__
#elif __MIKROC_PRO_FOR_ARM__
#else
#define ON_PC
#endif

#define field( NAME ) fields->tokens[ NAME ][ 0 ]
#define token( NAME ) fields->tokens[ NAME ]

/**************************
 * Globals
 * ***********************/
/* Used in Parsing */
static volatile char buffer[ BUFFER_MAX ];
static char process_buffer[ BUFFER_MAX ];
static volatile uint8_t buffer_position;
static volatile bool process_flag;

/* Global information recieved from several sentences */
static location_t cur_longitude;
static location_t cur_latitude;
static TimeStruct cur_time;
static utc_time_t cur_fix;

// Type of parsing to be done.
enum
{
    TIME,
    LOCATION_LAT,
    LOCATION_LON
};

// GGA fields
enum
{
    GGA_fix_tIME,
    GGA_LAT,
    GGA_LAT_AZMUTH,
    GGA_LON,
    GGA_LON_AZMUTH,
    GGA_FIX_QUALITY,
    GGA_NUM_SATS,
    GGA_HORT_DIL,
    GGA_ALT,
    GGA_METERS,
    GGA_HEIGHT,
    GGA_METERS2,
    GGA_LAST_UPD,
    GGA_STATION_ID
};
static gga_t cur_gga;

// GLL fields
enum
{
    GLL_LOCATION_LAT,
    GLL_LOCATION_LAT_AZMUTH,
    GLL_LOCATION_LON,
    GLL_LOCATION_LON_AZMUTH,
    GLL_fix_tIME,
    GLL_DATA_ACTIVE
};
static gll_t cur_gll;

// GSA fields
enum
{
    GSA_AUTO_SELECTION,
    GSA_DIM_FIX,
    GSA_SAT_1,
    GSA_SAT_2,
    GSA_SAT_3,
    GSA_SAT_4,
    GSA_SAT_5,
    GSA_SAT_6,
    GSA_SAT_7,
    GSA_SAT_8,
    GSA_SAT_9,
    GSA_SAT_10,
    GSA_SAT_11,
    GSA_SAT_12,
    GSA_PDOP,
    GSA_HDOP,
    GSA_VDOP
};
static gsa_t cur_gsa;

// GSV fields
enum
{
    GSV_NUM_SENTENCE,
    GSV_SENTENCE,
    GSV_NUM_SATS,
    GSV_SAT1_PRN,
    GSV_ELEVATION1,
    GSV_AZIMUTH1,
    GSV_SNR1,
    GSV_SAT2_PRN,
    GSV_ELEVATION2,
    GSV_AZIMUTH2,
    GSV_SNR2,
    GSV_SAT3_PRN,
    GSV_ELEVATION3,
    GSV_AZIMUTH3,
    GSV_SNR3,
    GSV_SAT4_PRN,
    GSV_ELEVATION4,
    GSV_AZIMUTH4,
    GSV_SNR4,
};
static gsv_t cur_gsv[3];

// RMC fields
enum
{
    RMC_FIX,
    RMC_STATUS,
    RMC_LAT,
    RMC_LAT_AZMUTH,
    RMC_LON,
    RMC_LON_AZMUTH,
    RMC_SPEED,
    RMC_TRACK,
    RMC_DATE,
    RMC_MAG,
    RMC_MAG_AZMUTH,
    RMC_MODE
};
static rmc_t cur_rmc;

// VTG fields
enum
{
    VTG_TRACK = 0,
    VTG_MAG_TRACK = 2,
    VTG_SPEED_KNOTS = 4,
    VTG_SPEED_KM
};
static vtg_t cur_vtg;

#ifdef DTM
// DTM fields
enum
{
    DTM_LOCAL_DATUM = 0,
    DTM_LOCAL_SUBCODE,
    DTM_LATITUDE_OFFSET,
    DTM_LATITUDE_OFFSET_MARK,
    DTM_LONGITUDE_OFFSET,
    DTM_LONGITUDE_OFFSET_MARK,
    DTM_ALTITUDE_OFFSET,
    DTM_DATUM
};
static dtm_t cur_dtm;
#endif
#ifdef GBS
// GBS fields
enum
{
    GBS_UTC = 0,
    GBS_LAT_ERROR,
    GBS_LON_ERROR,
    GBS_ALT_ERROR,
    GBS_FAILED_SAT_ID,
    GBS_PROB_MISS,
    GBS_FAILED_EST,
    GBS_STD_DEVIATION
};
static gbs_t cur_gbs;
#endif
#ifdef GPQ
static gpq_t cur_gpq;
#endif
#ifdef GRS
// GRS fields
enum
{
    GRS_UTC,
    GRS_MODE,
    GRS_RANGE
};
static grs_t cur_grs;
#endif
#ifdef GST
// GST fields
enum
{
    GST_UTC,
    GST_RMS,
    GST_STD_MAJ,
    GST_STD_MIN,
    GST_ORIENTATION,
    GST_STD_LAT,
    GST_STD_LON,
    GST_STD_ALT
};
static gst_t cur_gst;
#endif
#ifdef THS
static ths_t cur_ths;
#endif
#ifdef TXT
// TXT fields
enum
{
    TXT_TOTAL_PACKAGE = 0,
    TXT_MESSAGE_NUM,
    TXT_TYPE,
    TXT_MESSGE
};
static char txt_message[MAX_TXT_MESSAGE];
static txt_t cur_txt[MAX_TXT_PACKAGES];
#endif
#ifdef ZDA
// ZDA fields
enum
{
    ZDA_TIME,
    ZDA_DAY,
    ZDA_MONTH,
    ZDA_YEAR,
    ZDA_LOCAL_HOURS,
    ZDA_LOCAL_MINUTES
};
static zda_t cur_zda;
#endif

// buffer used in parsing sentence
typedef struct
{
    int8_t num_of_fields;
    char tokens[ MAX_FIELDS ][ MAX_FIELD_SIZE ];
} fields_t;


/************************************
 * Private Prototypes
 ***********************************/
static void process_gga( char *sentence );
static void process_gll( char *sentence );
static void process_gsa( char *sentence );
static void process_gsv( char *sentence );
static void process_rmc( char *sentence );
#ifdef DTM
static void process_dtm( char *sentence );
#endif
#ifdef GBS
static void process_gbs( char *sentence );
#endif
#ifdef GPQ
static void process_gpq( char *sentence );
#endif
#ifdef GRS
static void process_grs( char *sentence );
#endif
#ifdef GST
static void process_gst( char *sentence );
#endif
#ifdef THS
static void process_ths( char *sentence );
#endif
#ifdef TXT
static void process_txt( char *sentence );
#endif
#ifdef VTG
static void process_vtg( char *sentence );
#endif
#ifdef ZDA
static void process_zda( char *sentence );
#endif

// Router of sentence parsing
/* parses the sentence into fields */
static fields_t* parse_fields( char *sentence );
/* Removes leading 0 and returns float */
static double get_num_float( char *str );
/* Removes leading 0 and returns int */
static int get_num( char *str );
/* gets the time from string and populates time pointer */
static void get_time( char *str, utc_time_t *time );
/* gets time as well as date from string and populates pointer */
static void get_date( char *str, TimeStruct *ts );
/* Parses location both degrees, minutes, and azmuth */
static void get_location( char *str, location_t *location, int type );
/* for those platforms not found on the MikroC compiler */
static int xtoi( char *hexstring );
/* Utility function to calculate valid sentence */
static bool validate_checksum( char *sentence );
/* Main processing function */
static void gps_process_sentence( char *sentence );

/*********************
  Private Implimentations
*********************/
static void process_gga( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case GGA_fix_tIME:
            if( field( GGA_fix_tIME ) )
            {
                cur_gga.fix_time = &cur_fix;
                get_time( token( GGA_fix_tIME ), cur_gga.fix_time );
            }
            break;
        case GGA_LAT:
            if( field( GGA_LAT ) )
            {
                cur_gga.lat = &cur_latitude;
                get_location( token( GGA_LAT ), cur_gga.lat, LOCATION_LAT );
            }
            break;
        case GGA_LAT_AZMUTH:
            if( field( GGA_LAT_AZMUTH ) )
            {
                if( field( GGA_LAT_AZMUTH ) == 'N' )
                    cur_gga.lat->azmuth = NORTH;
                else if( field( GGA_LAT_AZMUTH ) == 'S' )
                    cur_gga.lat->azmuth = SOUTH;
                else
                    cur_gga.lat->azmuth = UNKNOWN;
            }
            break;
        case GGA_LON:
            if( field( GGA_LON ) )
            {
                cur_gga.lon = &cur_longitude;
                get_location( token( GGA_LON ), cur_gga.lon, LOCATION_LON );
            }
            break;
        case GGA_LON_AZMUTH:
            if( field( GGA_LON_AZMUTH ) )
            {
                if( field( GGA_LON_AZMUTH ) == 'E' )
                    cur_gga.lon->azmuth = EAST;
                else if( field( GGA_LON_AZMUTH ) == 'W' )
                    cur_gga.lon->azmuth = WEST;
                else
                    cur_gga.lon->azmuth = UNKNOWN;
            }
            break;
        case GGA_FIX_QUALITY:
            if( field( GGA_FIX_QUALITY ) )
                cur_gga.fix = get_num( token( GGA_FIX_QUALITY ) );
            break;
        case GGA_NUM_SATS:
            if( field( GGA_NUM_SATS ) )
                cur_gga.num_sats = get_num( token( GGA_NUM_SATS ) );
            break;
        case GGA_HORT_DIL:
            if( field( GGA_HORT_DIL ) )
                cur_gga.horizontal = get_num_float( token( GGA_HORT_DIL ) );
            break;
        case GGA_ALT:
            if( field( GGA_ALT ) )
                cur_gga.altitude = get_num_float( token( GGA_ALT ) );
            break;
        case GGA_HEIGHT:
            if( field( GGA_HEIGHT ) )
                cur_gga.height = get_num_float( token( GGA_HEIGHT ) );
            break;
        case GGA_LAST_UPD:
            if( field( GGA_LAST_UPD ) )
                cur_gga.last_update = get_num_float( token( GGA_LAST_UPD ) );
            break;
        case GGA_STATION_ID:
            if( field( GGA_STATION_ID ) )
                cur_gga.station_id = get_num_float( token( GGA_STATION_ID ) );
            break;
        };
    }
    return;
}

static void process_gll( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case GLL_LOCATION_LAT:
            if( field( GLL_LOCATION_LAT ) )
            {
                cur_gll.lat = &cur_latitude;
                get_location( token( GLL_LOCATION_LAT ), cur_gll.lon, LOCATION_LAT );
            }
            break;
        case GLL_LOCATION_LAT_AZMUTH:
            if( field( GLL_LOCATION_LAT_AZMUTH ) )
            {
                if( field( GLL_LOCATION_LAT_AZMUTH ) == 'N' )
                    cur_gll.lat->azmuth = NORTH;
                else if( field( GLL_LOCATION_LAT_AZMUTH ) == 'S' )
                    cur_gll.lat->azmuth = SOUTH;
                else
                    cur_gll.lat->azmuth = UNKNOWN;
            }
            break;
        case GLL_LOCATION_LON:
            if( field( GLL_LOCATION_LON ) )
            {
                cur_gll.lon = &cur_longitude;
                get_location( token( GLL_LOCATION_LON ), cur_gll.lon, LOCATION_LON );
            }
            break;
        case GLL_fix_tIME:
            if( field( GLL_fix_tIME ) )
            {
                cur_gll.fix_time = &cur_fix;
                get_time( token( GLL_fix_tIME ), cur_gll.fix_time );
            }
            break;
        case GLL_DATA_ACTIVE:
            if( field( GLL_DATA_ACTIVE ) )
            {
                if( field( GLL_DATA_ACTIVE ) == 'A' )
                    cur_gll.active = LORAN_ACTIVE;
                else if( field( GLL_DATA_ACTIVE ) == 'V' )
                    cur_gll.active = LORAN_VOID;
                else
                    cur_gll.active = LORAN_UNKNOWN;
            }
            break;
        };
    }
}

static void process_gsa( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case GSA_AUTO_SELECTION:
            if( field( GSA_AUTO_SELECTION ) )
            {
                if( field( GSA_AUTO_SELECTION ) == 'A' )
                    cur_gsa.mode = GSA_AUTO_MODE;
                else if( field( GSA_AUTO_SELECTION ) == 'M' )
                    cur_gsa.mode = GSA_MANUAL_MODE;
                else
                    cur_gsa.mode = GSA_UNKNOWN;
            }
            break;
        case GSA_DIM_FIX:
            if( field( GSA_DIM_FIX ) )
                cur_gsa.fix = get_num( token( GSA_DIM_FIX ) );
            break;
        case GSA_SAT_1:
            if( field( GSA_SAT_1 ) )
                cur_gsa.sats[ 0 ] = get_num( token( GSA_SAT_1 ) );
            break;
        case GSA_SAT_2:
            if( field( GSA_SAT_2 ) )
                cur_gsa.sats[ 1 ] = get_num( token( GSA_SAT_2 ) );
            break;
        case GSA_SAT_3:
            if( field( GSA_SAT_3 ) )
                cur_gsa.sats[ 2 ] = get_num( token( GSA_SAT_3 ) );
            break;
        case GSA_SAT_4:
            if( field( GSA_SAT_4 ) )
                cur_gsa.sats[ 3 ] = get_num( token( GSA_SAT_4 ) );
            break;
        case GSA_SAT_5:
            if( field( GSA_SAT_5 ) )
                cur_gsa.sats[ 4 ] = get_num( token( GSA_SAT_5 ) );
            break;
        case GSA_SAT_6:
            if( field( GSA_SAT_6 ) )
                cur_gsa.sats[ 5 ] = get_num( token( GSA_SAT_6 ) );
            break;
        case GSA_SAT_7:
            if( field( GSA_SAT_7 ) )
                cur_gsa.sats[ 6 ] = get_num( token( GSA_SAT_7 ) );
            break;
        case GSA_SAT_8:
            if( field( GSA_SAT_8 ) )
                cur_gsa.sats[ 7 ] = get_num( token( GSA_SAT_8 ) );
            break;
        case GSA_SAT_9:
            if( field( GSA_SAT_9 ) )
                cur_gsa.sats[ 8 ] = get_num( token( GSA_SAT_9 ) );
            break;
        case GSA_SAT_10:
            if( field( GSA_SAT_10 ) )
                cur_gsa.sats[ 9 ] = get_num( token( GSA_SAT_10 ) );
            break;
        case GSA_SAT_11:
            if( field( GSA_SAT_11 ) )
                cur_gsa.sats[ 10 ] = get_num( token( GSA_SAT_11 ) );
            break;
        case GSA_SAT_12:
            if( field( GSA_SAT_12 ) )
                cur_gsa.sats[ 11 ] = get_num( token( GSA_SAT_12 ) );
            break;
        case GSA_PDOP:
            if( field( GSA_PDOP ) )
                cur_gsa.pdop = get_num_float( token( GSA_PDOP ) );
            break;
        case GSA_HDOP:
            if( field( GSA_HDOP ) )
                cur_gsa.pdop = get_num_float( token( GSA_HDOP ) );
            break;
        case GSA_VDOP:
            if( field( GSA_VDOP ) )
                cur_gsa.pdop = get_num_float( token( GSA_VDOP ) );
            break;
        };
    }
    return;
}

static void process_gsv( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;
    gsv_t *cur_sentence = &cur_gsv[0];


    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case GSV_SENTENCE:
            if( field( GSV_SENTENCE ) )
            {
                uint8_t tmp_num = get_num( token( GSV_SENTENCE ) );

                if( tmp_num <= 3 && tmp_num >= 1 )
                    cur_sentence = &cur_gsv[tmp_num - 1];
                else
                    return;
            }
            break;
        case GSV_NUM_SATS:
            if( field( GSV_NUM_SATS ) )
                cur_sentence->num_sats = get_num( token( GSV_NUM_SATS) );
            break;
        case GSV_SAT1_PRN:
            if( field( GSV_SAT1_PRN) )
                cur_sentence->sat_info[0].sat_prn_num = get_num( token( GSV_SAT1_PRN ) );
            break;
        case GSV_ELEVATION1:
            if( field( GSV_ELEVATION1 ) )
                cur_sentence->sat_info[0].elevation = get_num( token( GSV_ELEVATION1 ) );
            break;
        case GSV_AZIMUTH1:
            if( field( GSV_AZIMUTH1 ) )
                cur_sentence->sat_info[0].azimuth = get_num( token( GSV_AZIMUTH1 ) );
            break;
        case GSV_SNR1:
            if( field( GSV_SNR1 ) )
                cur_sentence->sat_info[0].azimuth = get_num( token( GSV_SNR1 ) );
            break;
        case GSV_SAT2_PRN:
            if( field( GSV_SAT2_PRN) )
                cur_sentence->sat_info[1].sat_prn_num = get_num( token( GSV_SAT2_PRN ) );
            break;
        case GSV_ELEVATION2:
            if( field( GSV_ELEVATION2 ) )
                cur_sentence->sat_info[1].elevation = get_num( token( GSV_ELEVATION2 ) );
            break;
        case GSV_AZIMUTH2:
            if( field( GSV_AZIMUTH2 ) )
                cur_sentence->sat_info[1].azimuth = get_num( token( GSV_AZIMUTH2 ) );
            break;
        case GSV_SNR2:
            if( field( GSV_SNR2 ) )
                cur_sentence->sat_info[1].azimuth = get_num( token( GSV_SNR2 ) );
            break;
        case GSV_SAT3_PRN:
            if( field( GSV_SAT3_PRN) )
                cur_sentence->sat_info[2].sat_prn_num = get_num( token( GSV_SAT3_PRN ) );
            break;
        case GSV_ELEVATION3:
            if( field( GSV_ELEVATION3 ) )
                cur_sentence->sat_info[2].elevation = get_num( token( GSV_ELEVATION3 ) );
            break;
        case GSV_AZIMUTH3:
            if( field( GSV_AZIMUTH3 ) )
                cur_sentence->sat_info[2].azimuth = get_num( token( GSV_AZIMUTH3 ) );
            break;
        case GSV_SNR3:
            if( field( GSV_SNR3 ) )
                cur_sentence->sat_info[2].azimuth = get_num( token( GSV_SNR3 ) );
            break;
        case GSV_SAT4_PRN:
            if( field( GSV_SAT4_PRN) )
                cur_sentence->sat_info[3].sat_prn_num = get_num( token( GSV_SAT4_PRN ) );
            break;
        case GSV_ELEVATION4:
            if( field( GSV_ELEVATION4 ) )
                cur_sentence->sat_info[3].elevation = get_num( token( GSV_ELEVATION4 ) );
            break;
        case GSV_AZIMUTH4:
            if( field( GSV_AZIMUTH4 ) )
                cur_sentence->sat_info[3].azimuth = get_num( token( GSV_AZIMUTH4 ) );
            break;
        case GSV_SNR4:
            if( field( GSV_SNR4 ) )
                cur_sentence->sat_info[3].azimuth = get_num( token( GSV_SNR4 ) );
            break;
        };
    }
    return;
}


static void process_rmc( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case RMC_FIX:
            if( field( RMC_FIX ) )
            {
                cur_rmc.fix_time = &cur_fix;
                get_time( token( RMC_FIX ), cur_rmc.fix_time );
            }
            break;
        case RMC_STATUS:
            if( field( RMC_STATUS ) )
            {
                if( field( RMC_STATUS ) == 'A' )
                    cur_rmc.status = RMC_ACTIVE;
                else if( field( RMC_STATUS ) == 'V' )
                    cur_rmc.status = RMC_VOID;
                else
                    cur_rmc.status = RMC_UKNOWN;
            }
            break;
        case RMC_LAT:
            if( field( RMC_LAT ) )
            {
                cur_rmc.lat = &cur_latitude;
                get_location( token( RMC_LAT ), cur_rmc.lat, LOCATION_LAT );
            }
            break;
        case RMC_LAT_AZMUTH:
            if( field( RMC_LAT_AZMUTH ) )
            {
                if( field( RMC_LAT_AZMUTH ) == 'N' )
                    cur_rmc.lat->azmuth = NORTH;
                else if( field( RMC_LAT_AZMUTH ) == 'S' )
                    cur_rmc.lat->azmuth = SOUTH;
                else
                    cur_rmc.lat->azmuth = UNKNOWN;
            }
            break;
        case RMC_LON:
            if( field( RMC_LON ) )
            {
                cur_rmc.lon = &cur_longitude;
                get_location( token( RMC_LON ), cur_rmc.lon, LOCATION_LON );
            }
            break;
        case RMC_LON_AZMUTH:
            if( field( RMC_LON_AZMUTH ) )
            {
                if( field( RMC_LON_AZMUTH ) == 'W' )
                    cur_rmc.lon->azmuth = WEST;
                else if( field( RMC_LON_AZMUTH ) == 'E' )
                    cur_rmc.lon->azmuth = EAST;
                else
                    cur_rmc.lon->azmuth = UNKNOWN;
            }
            break;
        case RMC_SPEED:
            if( field( RMC_SPEED ) )
                cur_rmc.speed = get_num_float( token( RMC_SPEED ) );
            break;
        case RMC_TRACK:
            if( field( RMC_TRACK ) )
                cur_rmc.track = get_num_float( token( RMC_TRACK ) );
            break;
        case RMC_DATE:
            if( field( RMC_DATE ) )
            {
                cur_rmc.date = &cur_time;
                get_date( token( RMC_DATE ), cur_rmc.date );
            }
            break;
        case RMC_MAG:
            if( field( RMC_MAG ) )
                cur_rmc.magnetic.mag_variation = get_num_float( token( RMC_MAG ) );
            break;
        case RMC_MAG_AZMUTH:
            if( field( RMC_MAG_AZMUTH ) )
            {
                if( field( RMC_MAG_AZMUTH ) == 'N' )
                    cur_rmc.magnetic.azmuth = NORTH;
                else if( field( RMC_MAG_AZMUTH ) == 'S' )
                    cur_rmc.magnetic.azmuth = SOUTH;
                else if( field( RMC_MAG_AZMUTH ) == 'W' )
                    cur_rmc.magnetic.azmuth = WEST;
                else if( field( RMC_MAG_AZMUTH ) == 'E' )
                    cur_rmc.magnetic.azmuth = EAST;
                else
                    cur_rmc.magnetic.azmuth = UNKNOWN;
            }
            break;
        };
    }
    return;
}

static void process_vtg( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case VTG_TRACK:
            if( field( VTG_TRACK ) )
                cur_vtg.track = get_num_float( token( VTG_TRACK ) );
            break;
        case VTG_MAG_TRACK:
            if( field( VTG_MAG_TRACK ) )
                cur_vtg.track = get_num_float( token( VTG_MAG_TRACK ) );
            break;
        case VTG_SPEED_KNOTS:
            if( field( VTG_SPEED_KNOTS ) )
                cur_vtg.track = get_num_float( token( VTG_SPEED_KNOTS ) );
            break;
        case VTG_SPEED_KM:
            if( field( VTG_SPEED_KM ) )
                cur_vtg.track = get_num_float( token( VTG_SPEED_KM ) );
            break;
        };
    }
    return;
}

#ifdef DTM
static void process_dtm( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case DTM_LOCAL_DATUM:
            // Local datum code
            if( field( DTM_LOCAL_DATUM ) )
            {
                if( !strcmp( token( DTM_LOCAL_DATUM ), "W84" ) )
                    cur_dtm.local_datum = WGS84;
                else if( !strcmp( token( DTM_LOCAL_DATUM ), "W72" ) )
                    cur_dtm.local_datum = WGS72;
                else if( !strcmp( token( DTM_LOCAL_DATUM ), "S85" ) )
                    cur_dtm.local_datum = SGS85;
                else if( !strcmp( token( DTM_LOCAL_DATUM ), "P90" ) )
                    cur_dtm.local_datum = PE90;
                else if( !strcmp( token( DTM_LOCAL_DATUM ), "999" ) )
                    cur_dtm.local_datum = USER_DEFINED;
                else if( !strcmp( token( DTM_LOCAL_DATUM ), "IHO" ) )
                    cur_dtm.local_datum = IHO;
            }
            break;
        case DTM_LOCAL_SUBCODE:
            // Local datum sub-code
            if( field( DTM_LOCAL_SUBCODE ) )
                memcpy( cur_dtm.lsd, token( DTM_LOCAL_SUBCODE ), 1 );
            break;
        case DTM_LATITUDE_OFFSET:
            // Offset in latitude in minutes
            if( field( DTM_LATITUDE_OFFSET ) )
                cur_dtm.lat = get_num_float( token( DTM_LATITUDE_OFFSET ) );
            break;
        case DTM_LATITUDE_OFFSET_MARK:
            // North South Indication
            if( field( DTM_LATITUDE_OFFSET_MARK ) )
            {
                if( field( DTM_LATITUDE_OFFSET_MARK ) == 'N' )
                    cur_dtm.lat_offset_dir = NORTH;
                else if( field( DTM_LATITUDE_OFFSET_MARK ) == 'S' )
                    cur_dtm.lat_offset_dir = SOUTH;
                else
                    cur_dtm.lat_offset_dir = UNKNOWN;
            }
            break;
        case DTM_LONGITUDE_OFFSET:
            if( field( DTM_LONGITUDE_OFFSET ) )
                cur_dtm.lon = get_num_float( token( DTM_LONGITUDE_OFFSET ) );
            break;
        case DTM_LONGITUDE_OFFSET_MARK:
            if( field( DTM_LONGITUDE_OFFSET_MARK ) )
            {
                if( field( DTM_LONGITUDE_OFFSET_MARK ) == 'E' )
                    cur_dtm.lon_offset_dir = EAST;
                else if( field( DTM_LONGITUDE_OFFSET_MARK ) == 'W' )
                    cur_dtm.lon_offset_dir = WEST;
                else
                    cur_dtm.lon_offset_dir = UNKNOWN;
            }
            break;
        case DTM_ALTITUDE_OFFSET:
            if( field( DTM_ALTITUDE_OFFSET ) != '\0' )
                cur_dtm.alt = get_num_float( token( DTM_ALTITUDE_OFFSET ) );
            break;
        case DTM_DATUM:
            // Local datum code
            if( field( DTM_DATUM ) )
            {
                if( !strcmp( token( DTM_DATUM ), "W84" ) )
                    cur_dtm.datum = WGS84;
                else if( !strcmp( token( DTM_DATUM ), "W72" ) )
                    cur_dtm.datum = WGS72;
                else if( !strcmp( token( DTM_DATUM ), "S85" ) )
                    cur_dtm.datum = SGS85;
                else if( !strcmp( token( DTM_DATUM ), "P90" ) )
                    cur_dtm.datum = PE90;
                else if( !strcmp( token( DTM_DATUM ), "999" ) )
                    cur_dtm.datum = USER_DEFINED;
                else if( !strcmp( token( DTM_DATUM ), "IHO" ) )
                    cur_dtm.datum = IHO;
                else
                    cur_dtm.datum = DATUM_UNKNOWN;
            }
        };
    }
    return;
}
#endif

#ifdef GBS
static void process_gbs( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case GBS_UTC:
            if( field( GBS_UTC ) )
            {
                cur_gbs.fix_time = &cur_fix;
                get_time( token( GBS_UTC ), cur_gbs.fix_time );
            }
            break;
        case GBS_LAT_ERROR:
            if( field( GBS_LAT_ERROR ) )
                cur_gbs.lat_error = get_num_float( token( GBS_LAT_ERROR ) );
            break;
        case GBS_LON_ERROR:
            if( field( GBS_LON_ERROR ) )
                cur_gbs.lon_error = get_num_float( token( GBS_LON_ERROR ) );
            break;
        case GBS_ALT_ERROR:
            if( field( GBS_ALT_ERROR ) )
                cur_gbs.alt_error = get_num_float( token( GBS_ALT_ERROR ) );
            break;
        case GBS_FAILED_SAT_ID:
            if( field( GBS_FAILED_SAT_ID ) )
                cur_gbs.sat_id = get_num( token( GBS_FAILED_SAT_ID ) );
            break;
        case GBS_PROB_MISS:
            if( field( GBS_PROB_MISS ) )
                cur_gbs.prob_miss = get_num_float( token( GBS_PROB_MISS ) );
            break;
        case GBS_FAILED_EST:
            if( field( GBS_FAILED_EST ) )
                cur_gbs.failed_est = get_num_float( token( GBS_FAILED_EST ) );
            break;
        case GBS_STD_DEVIATION:
            if( field( GBS_STD_DEVIATION ) )
                cur_gbs.std_deviation = get_num_float( token( GBS_STD_DEVIATION ) );
            break;
        };
    }
}
#endif

#ifdef GPQ
static void process_gpq( char *sentence )
{
    fields_t *fields = parse_fields( sentence );

    if( fields->tokens[0][0] != '\0' )
        strcpy( cur_gpq.id, fields->tokens[0] );
    return;
}
#endif

#ifdef GRS
static void process_grs( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case GRS_UTC:
            if( field( GRS_UTC ) )
            {
                cur_grs.fix_time = &cur_fix;
                get_time( token( GRS_UTC ), cur_grs.fix_time );
            }
            break;
        case GRS_MODE:
            if( field( GRS_MODE ) )
                cur_grs.mode = get_num( token( GRS_MODE ) );
            break;
        case GRS_RANGE:
            if( field( GRS_RANGE ) )
                cur_grs.range = get_num_float( token( GRS_RANGE ) );
            break;
        }
    }
    return;
}
#endif

#ifdef GST
static void process_gst( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case GST_UTC:
            if( field( GST_UTC ) )
            {
                cur_gst.fix_time = &cur_fix;
                get_time( token( GST_UTC ), cur_gst.fix_time );
            }
            break;
        case GST_RMS:
            if( field( GST_RMS ) )
                cur_gst.rms = get_num_float( token( GST_RMS ) );
            break;
        case GST_STD_MAJ:
            if( field( GST_STD_MAJ ) )
                cur_gst.rms = get_num_float( token( GST_STD_MAJ ) );
            break;
        case GST_STD_MIN:
            if( field( GST_STD_MIN ) )
                cur_gst.rms = get_num_float( token( GST_STD_MIN ) );
            break;
        case GST_ORIENTATION:
            if( field( GST_ORIENTATION ) )
                cur_gst.rms = get_num_float( token( GST_ORIENTATION ) );
            break;
        case GST_STD_LAT:
            if( field( GST_STD_LAT ) )
                cur_gst.rms = get_num_float( token( GST_STD_LAT ) );
            break;
        case GST_STD_LON:
            if( field( GST_STD_LON ) )
                cur_gst.rms = get_num_float( token( GST_STD_LON ) );
            break;
        case GST_STD_ALT:
            if( field( GST_STD_ALT ) )
                cur_gst.rms = get_num_float( token( GST_STD_ALT ) );
            break;
        };
    }
    return;
}
#endif

#ifdef THS
static void process_ths( char *sentence )
{
    fields_t *fields = parse_fields( sentence );

    if( fields->tokens[ 0 ][ 0 ] != '\0' )
        cur_ths.heading = get_num_float( fields->tokens[ 0 ] );

    if( fields->tokens[ 1 ][ 0 ] != '\0' )
    {
        if( fields->tokens[ 1 ][ 0 ] == 'A' )
            cur_ths.status = VEHICLE_AUTONOMOUS;
        else if( fields->tokens[ 1 ][ 0 ] == 'E' )
            cur_ths.status = VEHICLE_ESTIMATED;
        else if( fields->tokens[ 1 ][ 0 ] == 'M' )
            cur_ths.status = VEHICLE_MANUAL;
        else if( fields->tokens[ 1 ][ 0 ] == 'S' )
            cur_ths.status = VEHICLE_SIMULATOR;
        else if( fields->tokens[ 1 ][ 0 ] == 'V' )
            cur_ths.status = VEHICLE_NOT_VALID;
        else
            cur_ths.status = VEHICLE_UKNOWN;
    }

    return;
}
#endif

#ifdef TXT
static void process_txt( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;
    txt_t *tmptxt = &cur_txt[0];

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case TXT_TOTAL_PACKAGE:
            if( field( TXT_TOTAL_PACKAGE ) )
                tmptxt->num_of_mesg = get_num( token( TXT_TOTAL_PACKAGE ) );
            break;
        case TXT_MESSAGE_NUM:
            if( field( TXT_MESSAGE_NUM ) )
            {
                uint8_t tmpnum = get_num( token( TXT_MESSAGE_NUM) );

                if( tmpnum <= MAX_TXT_PACKAGES )
                {
                    tmptxt = &cur_txt[ tmpnum -1 ];
                }
            }
            break;
        case TXT_TYPE:
            if( field( TXT_TYPE ) )
                tmptxt->mesg_type = get_num( token( TXT_TYPE ) );
            break;
        case TXT_MESSGE:
            if( field( TXT_MESSGE ) )
                strcpy( tmptxt->mesg, token( TXT_MESSGE ) );
            break;
        };
    }

    return;
}
#endif

#ifdef ZDA
static void process_zda( char *sentence )
{
    fields_t *fields = parse_fields( sentence );
    int i;

    cur_zda.time = &cur_time;

    for( i = 0; i < fields->num_of_fields; i++ )
    {
        switch( i )
        {
        case ZDA_TIME:
            if( field( ZDA_TIME ) )
            {
                utc_time_t tmp_time;
                get_time( token( ZDA_TIME ), &tmp_time );
                cur_zda.time->hh = tmp_time.hour;
                cur_zda.time->mn = tmp_time.minute;
                cur_zda.time->ss = tmp_time.second;
            }
            break;
        case ZDA_DAY:
            if( field( ZDA_DAY ) )
                cur_zda.time->md = get_num( token( ZDA_DAY ) );
            break;
        case ZDA_MONTH:
            if( field( ZDA_MONTH ) )
                cur_zda.time->mo = get_num( token( ZDA_MONTH ) );
            break;
        case ZDA_YEAR:
            if( field( ZDA_YEAR ) )
                cur_zda.time->yy = get_num( token( ZDA_YEAR ) );
            break;
        case ZDA_LOCAL_HOURS:
            if( field( ZDA_LOCAL_HOURS ) )
                cur_zda.local_hour = get_num( token( ZDA_LOCAL_HOURS ) );
            break;
        case ZDA_LOCAL_MINUTES:
            if( field( ZDA_LOCAL_MINUTES ) )
                cur_zda.local_min = get_num( token( ZDA_LOCAL_MINUTES ) );
            break;
        };
    }
    return;
}
#endif


static fields_t* parse_fields( char *sentence )
{
    static fields_t tmp_fields;
    char *p_sentence = sentence;
    char *p_next = strchr( p_sentence, '*' );

    tmp_fields.num_of_fields = 0;
    memset( &tmp_fields, 0, sizeof( fields_t ) );

    *p_next = '\0'; // Replace start of checksum with null
    p_sentence = strchr( p_sentence, ',' ); /* Moves us to the first, which is just past the identifier */

    do
    {
        p_sentence++;
        p_next = strchr( p_sentence, ',' );     /* Gets the next , so we can calculate the number of bytes to copy */

        if( p_next != 0 )
            memcpy( tmp_fields.tokens[tmp_fields.num_of_fields++], p_sentence, p_next - p_sentence );
        else
            strcpy( tmp_fields.tokens[tmp_fields.num_of_fields++], p_sentence );

        p_sentence = p_next;
    }
    while( p_sentence != 0 );

    return &tmp_fields;
}


static double get_num_float( char *str )
{
    int n;
    double num;
    char *tmp = str;

    if( ( n = strspn( tmp, "0" ) ) != 0 && tmp[n] != '\0' )
        num = atof( &tmp[n] );
    else
        num = atof( tmp );

    return num;
}

static int get_num( char *str )
{
    int n, num;
    char *tmp = str;

    if( ( n = strspn( tmp, "0" ) ) != 0 && tmp[n] != '\0' )
        num = atoi( &tmp[n] );
    else
        num = atoi( tmp );

    return num;
}

static void get_time( char *str, utc_time_t *time )
{
    char tmp[4] = {0}, *p_tmp = str;
    int i, runcount;
    void *tmp_time = ( void* )time;

    if( strchr( str, '.' ) )
        runcount = 4;
    else
        runcount = 3;

    for( i = 0; i < runcount; i++ )
    {
        if( *p_tmp == '.' )
        {
            p_tmp++;
            strncpy( tmp, p_tmp, 3 );
            tmp[3] = 0;
        }
        else
        {
            strncpy( tmp, p_tmp, 2 );
            tmp[2] = 0;
        }

        *( uint8_t* )tmp_time = get_num( tmp );

        p_tmp += 2;

        ( uint8_t* )tmp_time++;
    }

    return;
}

static void get_date( char *str, TimeStruct *ts )
{
    char tmp[3] = {0};
    char *p_str = str;

    strncpy( tmp, p_str, 2 );
    ts->md = get_num( tmp );
    p_str += 2;
    strncpy( tmp, p_str, 2 );
    ts->mn = get_num( tmp );
    p_str += 2;
    strncpy( tmp, p_str, 2 );
    ts->yy = 2000 + get_num( tmp );

    return;
}

static void get_location( char *str, location_t *location, int type )
{
    if( location != 0 )
    {
        char tmp[10] = {0}, *p_tmp = str;

        if( type == LOCATION_LAT )
        {
            strncpy( tmp, p_tmp, 2 );
            p_tmp += 2;
        }
        else
        {
            strncpy( tmp, p_tmp, 3 );
            p_tmp += 3;
        }

        location->degrees = get_num( tmp );
        strcpy( tmp, p_tmp );
        location->minutes = get_num( tmp );
    }

    return;
}

// Only needed on platforms other than mikroC
#ifdef ON_PC
static int xtoi( char *hexstring )
{
    int i = 0;

    if( ( *hexstring == '0' ) && ( *( hexstring + 1 ) == 'x' ) )
        hexstring += 2;

    while( *hexstring )
    {
        char c = toupper( *hexstring++ );

        if( (c < '0') || ( c > 'F' ) || ( ( c > '9' ) && ( c < 'A' ) ) )
            break;

        c -= '0';

        if( c > 9 )
            c -= 7;
        i = ( i << 4 ) + c;
    }

    return i;
}
#endif

// Check checksum of incoming sentences
bool validate_checksum( char *sentence )
{
    bool flagValid = true;
    char text[80];

    if( sentence[ 0 ] != '$' )
    {
        flagValid = false;
    }

    // if we are still good, test all bytes
    if( flagValid == true )
    {
        uint8_t position = 1;
        uint8_t chksum, nmeaChk;
        char current_char;
        char hx[5] = "0x00";

        current_char = sentence[position++]; // get first chr
        chksum = current_char;

        while( ( current_char != '*' ) && ( position < BUFFER_MAX ) )
        {
            current_char = sentence[ position ]; // get next chr

            if( current_char != '*' )
            {
                chksum = chksum ^ current_char;
            }

            position++;
        }

        // at this point we are either at * or at end of string
#ifdef ON_PC
        hx[2] = sentence[ position ];
        hx[3] = sentence[ position + 1 ];
        hx[4] = '\0';
#else
        hx[0] = sentence[ position ];
        hx[1] = sentence[ position + 1 ];
        hx[2] = '\0';
#endif

        nmeaChk = xtoi( hx );


        if( chksum != nmeaChk )
        {
            flagValid = false;
        }

    }

    return flagValid;
}

// Router for incoming complete sentences
static void gps_process_sentence( char *sentence )
{
#define MAX_COMPARE 5
    char *process_sentence = sentence;

    if( !validate_checksum( process_sentence ) )
        return;

    if( !strncmp( sentence, "$GPGGA", MAX_COMPARE ) )
        process_gga( process_sentence );
    else if( !strncmp( sentence, "$GPGGA", MAX_COMPARE ) )
        process_gga( process_sentence );
    else if( !strncmp( sentence, "$GPGLL", MAX_COMPARE ) )
        process_gll( process_sentence );
    else if( !strncmp( sentence, "$GPGSA", MAX_COMPARE ) )
        process_gsa( process_sentence );
    else if( !strncmp( sentence, "$GPGSV", MAX_COMPARE ) )
        process_gsv( process_sentence );
    else if( !strncmp( sentence, "$GPRMC", MAX_COMPARE ) )
        process_rmc( process_sentence );
    else if( !strncmp( sentence, "$GPVTG", MAX_COMPARE ) )
        process_vtg( process_sentence );
#ifdef DTM
    else if( !strncmp( sentence, "$GPDTM", MAX_COMPARE ) )
        process_dtm( process_sentence );
#endif
#ifdef GBS
    else if( !strncmp( sentence, "$GPGBS", MAX_COMPARE ) )
        process_gbs( process_sentence );
#endif
#ifdef GPQ
    else if( !strncmp( sentence, "$GPGPQ", MAX_COMPARE ) )
        process_gpq( process_sentence );
#endif
#ifdef GRS
    else if( !strncmp( sentence, "$GPGRS", MAX_COMPARE ) )
        process_grs( process_sentence );
#endif
#ifdef GST
    else if( !strncmp( sentence, "$GPGST", MAX_COMPARE ) )
        process_gst( process_sentence );
#endif
#ifdef THS
    else if( !strncmp( sentence, "$GPTHS", MAX_COMPARE ) )
        process_ths( process_sentence );
#endif
#ifdef TXT
    else if( !strncmp( sentence, "$GPTXT", MAX_COMPARE ) )
        process_txt( process_sentence );
#endif
#ifdef ZDA
    else if( !strncmp( sentence, "$GPZDA", MAX_COMPARE ) )
        process_zda( process_sentence );
#endif
    return;
}



/*******************************
 *     Public Functions
 * ****************************/
void gps_put( char input )
{
    static bool sentence_flag;

    if( ( input != '\r' && input != '\n' ) && buffer_position < BUFFER_MAX )
    {
        buffer[ buffer_position++ ] = input;
    }
    else if( input == '\r' )
    {
        sentence_flag = true;
    }
    else if( input == '\n' && sentence_flag )
    {
        buffer[ buffer_position ] = '\0';
        buffer_position = 0;
        strcpy( process_buffer, buffer );
        sentence_flag = false;
        process_flag = true;
    }
    else
    {
        buffer_position = 0; /* invalid something or other */
    }
}

void gps_parse()
{
    if( process_flag )
    {
        gps_process_sentence( process_buffer );
        process_flag = false;
    }
    return;
}

/***************** Common ***************/
location_t* gps_current_lon()
{
    return &cur_longitude;
}

location_t* gps_current_lat()
{
    return &cur_latitude;
}

TimeStruct* gps_current_time()
{
    return &cur_time;
}

utc_time_t* gps_current_fix()
{
    return &cur_fix;
}

/****************** GGA ******************/
fix_t gps_gga_fix_quality()
{
    return cur_gga.fix;
}

uint8_t gps_gga_satcount()
{
    return cur_gga.num_sats;
}

float gps_gga_hor_dilution()
{
    return cur_gga.horizontal;
}

double gps_gga_altitude()
{
    return cur_gga.altitude;
}

double gps_gga_msl()
{
    return cur_gga.height;
}

uint16_t gps_gga_lastDGPS_update()
{
    return cur_gga.last_update;
}

uint16_t gps_gga_DGPS_stationID()
{
    return cur_gga.station_id;
}

/***************** GLL ******************/
ACTIVE_t gps_gll_active()
{
    return cur_gll.active;
}

/***************** GSA ******************/
GSA_MODE_t gps_gsa_mode()
{
    return cur_gsa.mode;
}

GSA_MODE_t gps_gsa_fix_type()
{
    return cur_gsa.fix;
}

uint8_t *gps_gsa_sat_prn()
{
    return cur_gsa.sats;
}

float gps_gsa_precision_dilution()
{
    return cur_gsa.pdop;
}

float gps_gsa_horizontal_dilution()
{
    return cur_gsa.hdop;
}

float gps_gsa_vertical_dilution()
{
    return cur_gsa.vdop;
}

/***************** GSV *****************/
// TODO: Must combine sat info and clear
/***************** RMC *****************/
GPS_STATUS_t gps_rmc_status()
{
    return cur_rmc.status;
}

double gps_rmc_speed()
{
    return cur_rmc.speed;
}

double gps_rmc_track()
{
    return cur_rmc.track;
}

double gps_rmc_mag_var()
{
    return cur_rmc.magnetic.mag_variation;
}

azmuth_t gps_rmc_direction()
{
    return cur_rmc.magnetic.azmuth;
}

GPS_STATUS_t gps_rmc_mode()
{
    return cur_rmc.mode;
}

/**************** VTG *****************/
double gps_vtg_track()
{
    return cur_vtg.track;
}

double gps_vtg_mag()
{
    return cur_vtg.mag_track;
}

double gps_vtg_speedknt()
{
    return cur_vtg.speed_knots;
}

double gps_vtg_speedkm()
{
    return cur_vtg.speed_km;
}

#ifdef DTM
datum_code_t gps_dtm_local()
{
    return cur_dtm.local_datum;
}

char *gps_dtm_localoffset()
{
    return cur_dtm.lsd;
}

double gps_dtm_latoffset()
{
    return cur_dtm.lat;
}

azmuth_t gps_dtm_lat_offset_dir()
{
    return cur_dtm.lat_offset_dir;
}

double gps_dtm_lonoffset()
{
    return cur_dtm.lon;
}

azmuth_t gps_dtm_lon_offset_dir()
{
    return cur_dtm.lon_offset_dir;
}

double gps_dtm_altoffset()
{
    return cur_dtm.alt;
}

datum_code_t gps_dtm_datum()
{
    return cur_dtm.datum;
}
#endif

#ifdef GBS
float gps_gbs_laterror()
{
    return cur_gbs.lat_error;
}

float gps_gbs_lonerror()
{
    return cur_gbs.lon_error;
}

float gps_gbs_alterror()
{
    return cur_gbs.alt_error;
}

uint8_t gps_gbs_satid()
{
    return cur_gbs.sat_id;
}

float gps_gbs_probmiss()
{
    return cur_gbs.prob_miss;
}

double gps_gbs_failedest()
{
    return cur_gbs.failed_est;
}

float gps_gbs_std_deviation()
{
    return cur_gbs.std_deviation;
}
#endif

#ifdef GPQ
char *gps_gpq_message()
{
    return cur_gpq.id;
}

#endif

#ifdef GRS
uint8_t gps_grs_mode()
{
    return cur_grs.mode;
}

float gps_grs_range()
{
    return cur_grs.range;
}
#endif

#ifdef GST
float gps_gst_rms()
{
    return cur_gst.rms;
}

float gps_gst_stddev_major()
{
    return cur_gst.std_dev_maj;
}

float gps_gst_stddev_minor()
{
    return cur_gst.std_dev_min;
}

float gps_gst_orientation()
{
    return cur_gst.orientation;
}

float gps_gst_stddev_lat()
{
    return cur_gst.std_dev_lat;
}

float gps_gst_stddev_lon()
{
    return cur_gst.std_dev_lon;
}

float gps_gst_stddev_alt()
{
    return cur_gst.std_dev_alt;
}
#endif

#ifdef THS
double gps_ths_heading()
{
    return cur_ths.heading;
}

vehicle_status_t gps_ths_status()
{
    return cur_ths.status;
}

#endif

#ifdef TXT
// TODO:
#endif

#ifdef ZDA
uint8_t gps_zda_local_hour( void );
uint8_t gps_zda_local_min( void );
#endif