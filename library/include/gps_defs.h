/****************************************************************************
* Title                 :   GPS Common Definitions
* Filename              :   gps_defs.h
* Author                :   RL
* Origin Date           :   08/25/2015
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date    Software Version    Initials   Description
*  08/25/15    1.0               RL         Initial testing
*
*****************************************************************************/
/**
 * @file gps_defs.h
 * @brief GPS sentence definitions
 *
 * @date 25 Aug 2015
 * @author Richard Lowe
 * @copyright GNU Public License
 *
 * @version .1 - Initial testing and verification
 *
 * @note Test configuration:
 *  MCU:             STM32F107VC
 *  Dev.Board:       EasyMx Pro v7
 *  Oscillator:      72 Mhz internal
 *  Ext. Modules:    GPS Click
 *  SW:              ARM 4.5.2
 *
 */
#ifndef _GPS_DEFS_H
#define _GPS_DEFS_H

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include "time.h"
#include "gps_config.h"
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
#define MAX_FIELDS 19
#define MAX_FIELD_SIZE 10
#define BUFFER_MAX 80

#ifdef TXT
#define MAX_TXT_PACKAGES 3
#define MAX_TXT_SIZE 20
#define MAX_TXT_MESSAGE ( 3 * MAX_TXT_SIZE ) + 1
#endif
/******************************************************************************
* Configuration Constants
*******************************************************************************/


/******************************************************************************
* Macros
*******************************************************************************/



/******************************************************************************
* Typedefs
*******************************************************************************/
/**
 * @enum Azmuth
 * Holds common directions
 */
typedef enum
{
    UNKNOWN = 0,
    NORTH,
    SOUTH,
    EAST,
    WEST
} azmuth_t;

/**
 * @struct Latitude
 * Locational components represented in
 * degrees, minutes, and azmuth.
 * e.g 48 deg 07.038 N
 */
typedef struct
{
    uint8_t degrees; /**< Degrees 0-180 */
    double minutes;  /**< Minutes and seconds */
    azmuth_t azmuth; /**< N,E,S,W */
} location_t;

/**
 * @struct utc_time_t
 * @brief UTC Time representation.
 */
typedef struct
{
    uint8_t hour;   /**< Hour in 24 hour */
    uint8_t minute; /**< Minutes */
    uint8_t second; /**< Seconds */
    uint8_t ms;     /**< Miliseconds */
} utc_time_t;

/**
 * @enum Valid Satellite Fix Codes
 */
typedef enum
{
    INVALID,            /**< Invalid */
    GPS_FIX,            /**< GPS fix ( SPS ) */
    DGPS_FIX,           /**< DGPS fix */
    PPS_FIX,            /**< PPS fix */
    REAL_TIME_KINEMATIC,/**< Real Time Kinematic */
    FLOAT_RTK,          /**< Float RTK */
    ESTIMATED,          /**< Estimated ( dead recononing ) ( 2.3 feature ) */
    MANUAL_MODE,        /**< Manual input mode */
    SIMULATION_MODE     /**< Simulation mode */
} fix_t;


/**
 * @struct Definistion for GGA
 *
 * The most important NMEA sentences include the GGA which provides the current
 * Fix data, the RMC which provides the minimum gps sentences information, and the
 * GSA which provides the Satellite status data.
 *
 * GGA - essential fix data which provide 3D location and accuracy data.
 *
 * $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
 *
 * Where:
 *   GGA          Global Positioning System Fix Data
 *   123519       Fix taken at 12:35:19 UTC
 *   4807.038,N   Latitude 48 deg 07.038' N
 *   01131.000,E  Longitude 11 deg 31.000' E
 *   1            Fix quality: 0 = invalid
 *                             1 = GPS fix (SPS)
 *                             2 = DGPS fix
 *                             3 = PPS fix
 *                             4 = Real Time Kinematic
 *                             5 = Float RTK
 *                             6 = estimated (dead reckoning) (2.3 feature)
 *                             7 = Manual input mode
 *                             8 = Simulation mode
 *   08           Number of satellites being tracked
 *   0.9          Horizontal dilution of position
 *   545.4,M      Altitude, Meters, above mean sea level
 *   46.9,M       Height of geoid (mean sea level) above WGS84
 *                ellipsoid
 *   (empty field) time in seconds since last DGPS update
 *   (empty field) DGPS station ID number
 *   *47          the checksum data, always begins with *
 * If the height of geoid is missing then the altitude should be suspect. Some
 * non-standard implementations report altitude with respect to the ellipsoid
 * rather than geoid altitude. Some units do not report negative altitudes at all.
 * This is the only sentence that reports altitude.
 */
typedef struct
{
    utc_time_t *fix_time;  /**< Fix taken at 12:35:19 UTC */
    location_t *lat;       /**< Latitude 48 deg 07.038' N */
    location_t *lon;       /**< Longitude 11 deg 31.000' E */
    fix_t fix;             /**< Fix quality */
    uint8_t num_sats : 4;  /**< Number of satellites being tracked  */
    float horizontal;      /**< Horizontal dilution of position */
    double altitude;       /**< 545.4,M Altitude, Meters, above mean sea level */
    double height;         /**< Height of geoid (mean sea level) above WGS84 ellipsoid */
    uint16_t last_update;  /**< (empty field) time in seconds since last DGPS update */
    uint16_t station_id;   /**< (empty field) DGPS station ID number */
} gga_t;


/**
 * @enum Valid Loran status codes
 */
typedef enum
{
    LORAN_UNKNOWN,
    LORAN_ACTIVE,
    LORAN_VOID
} ACTIVE_t;

/**
 * @struct gll_t
 *
 * GLL - Geographic Latitude and Longitude is a holdover from Loran data and some
 * old units may not send the time and data active information if they are
 * emulating Loran data. If a gps is emulating Loran data they may use the LC
 * Loran prefix instead of GP.
 *
 *      $GPGLL,4916.45,N,12311.12,W,225444,A,*1D
 *
 * Where:
 *      GLL          Geographic position, Latitude and Longitude
 *      4916.46,N    Latitude 49 deg. 16.45 min. North
 *      12311.12,W   Longitude 123 deg. 11.12 min. West
 *      225444       Fix taken at 22:54:44 UTC
 *      A            Data Active or V (void)
 *      *iD          checksum data
 */
typedef struct
{
    location_t *lat;
    location_t *lon;
    utc_time_t *fix_time;/**< Fix taken at 22:54:44 UTC */
    ACTIVE_t active;    /**< Data Active or V (void) */
} gll_t;

/**
 * @enum Valid Satellite Fix Codes
 */
typedef enum
{
    GSA_UNKNOWN = 0,
    GSA_NO_FIX,     /**< 1 = no fix */
    GSA_2D_FIX,     /**< 2 = 2D fix */
    GSA_3D_FIX,     /**< 3 = 3D fix */
    GSA_AUTO_MODE,
    GSA_MANUAL_MODE
} GSA_MODE_t;

/**
 * @struct GSA Definition
 *
 * GSA - GPS DOP and active satellites. This sentence provides details on the
 * nature of the fix. It includes the numbers of the satellites being used in the
 * current solution and the DOP. DOP (dilution of precision) is an indication of
 * the effect of satellite geometry on the accuracy of the fix. It is a unitless
 * number where smaller is better. For 3D fixes using 4 satellites a 1.0 would be
 * considered to be a perfect number, however for overdetermined solutions it is
 * possible to see numbers below 1.0.
 *
 * There are differences in the way the PRN's are presented which can effect the
 * ability of some programs to display this data. For example, in the example
 * shown below there are 5 satellites in the solution and the null fields are
 * scattered indicating that the almanac would show satellites in the null
 * positions that are not being used as part of this solution. Other receivers
 * might output all of the satellites used at the beginning of the sentence with
 * the null field all stacked up at the end. This difference accounts for some
 * satellite display programs not always being able to display the satellites
 * being tracked. Some units may show all satellites that have ephemeris data
 * without regard to their use as part of the solution but this is non-standard.
 *
 * $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39
 *
 * Where:
 *    GSA      Satellite status
 *    A        Auto selection of 2D or 3D fix (M = manual)
 *    3        3D fix - values include: 1 = no fix
 *                                      2 = 2D fix
 *                                      3 = 3D fix
 *    04,05... PRNs of satellites used for fix (space for 12)
 *    2.5      PDOP (dilution of precision)
 *    1.3      Horizontal dilution of precision (HDOP)
 *    2.1      Vertical dilution of precision (VDOP)
 *    *39      the checksum data, always begins with *
 *
 */
typedef struct
{
    GSA_MODE_t mode;    /**< Auto selection of 2D or 3D fix ( M = manual ) */
    GSA_MODE_t fix;
    uint8_t sats[12];   /**< PRNs of satellites used for fix ( space for 12 )*/
    float pdop;         /**< PDOP ( dilution of precision ) */
    float hdop;         /**< Horizontal dilution of precision ( HDOP ) */
    float vdop;         /**< Vertical dilution of precision ( VDOP ) */
} gsa_t;

/**
 * @struct gsv
 *
 * GSV - Satellites in View shows data about the satellites that the unit might
 * be able to find based on its viewing mask and almanac data. It also shows
 * current ability to track this data. Note that one GSV sentence only can provide
 * data for up to 4 satellites and thus there may need to be 3 sentences for the
 * full information. It is reasonable for the GSV sentence to contain more
 * satellites than GGA might indicate since GSV may include satellites that are
 * not used as part of the solution. It is not a requirment that the GSV sentences
 * all appear in sequence. To avoid overloading the data bandwidth some receivers
 * may place the various sentences in totally different samples since each sentence
 * identifies which one it is.
 *
 * The field called SNR (Signal to Noise Ratio) in the NMEA standard is often referred to as signal strength. SNR is an indirect but more useful value that raw signal strength. It can range from 0 to 99 and has units of dB according to the NMEA standard, but the various manufacturers send different ranges of numbers with different starting numbers so the values themselves cannot necessarily be used to evaluate different units. The range of working values in a given gps will usually show a difference of about 25 to 35 between the lowest and highest values, however 0 is a special case and may be shown on satellites that are in view but not being tracked.
 *
 * $GPGSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45*75
 *
 * Where:
 *     GSV          Satellites in view
 *     2            Number of sentences for full data
 *     1            sentence 1 of 2
 *     08           Number of satellites in view
 *
 *     01           Satellite PRN number
 *     40           Elevation, degrees
 *     083          Azimuth, degrees
 *     46           SNR - higher is better
 *          for up to 4 satellites per sentence
 *     *75          the checksum data, always begins with *
 *
 */
typedef struct
{
    uint8_t num_sentences : 2; /**< Number of sentences for full data */
    uint8_t sentence : 2;      /**< Sentence number */
    uint8_t num_sats : 6;      /**< Number of satellites in view */
    struct
    {
        uint8_t sat_prn_num;   /**< Satellite PRN number */
        uint8_t elevation;     /**< Elevation in degrees */
        uint8_t azimuth;       /**< Azimuth in degrees */
        uint8_t snr;           /**< SNR ( Signal to noise ratio ) higher is better */
    } sat_info[4];
} gsv_t;

/**
 * @enum RMC Status
 */
typedef enum
{
    RMC_UKNOWN,
    RMC_ACTIVE,
    RMC_VOID,
    RMC_AUTONOMOUS,
    RMC_DIFFERENTIAL,
    RMC_NOT_VALID
} GPS_STATUS_t;

/**
 *  @struct The Recommended Minimum
 *
 * RMC - NMEA has its own version of essential gps pvt (position, velocity, time)
 * data. It is called RMC, The Recommended Minimum, which will look similar to:
 *
 *      $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
 *
 * Where:
 *      RMC          Recommended Minimum sentence C
 *      123519       Fix taken at 12:35:19 UTC
 *      A            Status A=active or V=Void.
 *      4807.038,N   Latitude 48 deg 07.038' N
 *      01131.000,E  Longitude 11 deg 31.000' E
 *      022.4        Speed over the ground in knots
 *      084.4        Track angle in degrees True
 *      230394       Date - 23rd of March 1994
 *      003.1,W      Magnetic Variation
 *      *6A          The checksum data, always begins with *
 */
typedef struct
{
    utc_time_t *fix_time; /**< Fix taken at 12:35:19 UTC */

    GPS_STATUS_t status; /**< Status A=active or V=Void. */
    location_t *lat;
    location_t *lon;
    double speed;       /**< Speed over the ground in knots */
    double track;       /**< Track angle in degrees True */
    TimeStruct *date;   /**< Date - 23rd of March 1994 */

    struct
    {
        double mag_variation; /**< Magnetic Variation */
        azmuth_t azmuth;
    } magnetic;

    GPS_STATUS_t mode;

} rmc_t;

/**
 * @struct Velocity made good
 *
 * VTG - Velocity made good. The gps receiver may use the LC prefix instead of GP
 * if it is emulating Loran output.
 *
 *    $GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48
 *
 * where:
 *       VTG          Track made good and ground speed
 *       054.7,T      True track made good (degrees)
 *       034.4,M      Magnetic track made good
 *       005.5,N      Ground speed, knots
 *       010.2,K      Ground speed, Kilometers per hour
 *       *48          Checksum
 */
typedef struct
{
    double track;    /**< 054.7,T True track made good (degrees) */
    double mag_track;/**< 034.4,M Magnetic track made good */
    double speed_knots;
    double speed_km; /**< 010.2,K Ground speed, Kilometers per hour */
} vtg_t;

#ifdef DTM
/**
 * @enum Datum Codes
 * Valid datum codes
 */
typedef enum
{
    DATUM_UNKNOWN = 0,
    WGS84,
    WGS72,
    SGS85,
    PE90,
    USER_DEFINED,
    IHO
} datum_code_t;

/**
 * @struct Datum information
 *
 * This message gives the difference between the currently selected Datum, and the reference
 * Datum.  If the currently configured Datum is not WGS84 or WGS72, then the field LLL will be set to
 * 999, and the field LSD is set to a variable-length string, representing the Name of the
 * Datum.
 *     $GPDTM,LLL,LSD,lat,N/S,lon,E/W,alt,RRR*cs<CR><LF>
 *     $GPDTM,W72,,0.00,S,0.01,W,-2.8,W84*4F
 * Where:
 *     DTM - Datum Reference
 *     W72 - Local Datum Code, W84 = WGS84, W72 = WGS72, 999 = user defined
 *     ,,  - Local Datum Subdivision Code, This field outputs the currently selected Datum as a string
 *     0.00- Offset in Latitude in minutes
 *     S   - North/South indicator
 *     0.01- Offset in Longitude in minutes
 *     W   - East/West indicator
 *    -2.8- Offset in altitude
 *     W84 - Reference Datum Code, W84 = WGS 84
 *     *4F - Checksum
 */
typedef struct
{
    datum_code_t local_datum;   /**< Local datum code */
    char lsd[2];                /**< Local datum subdivision */
    double lat;                 /**< Offset in latitude in minutes */
    azmuth_t lat_offset_dir;    /**< North / South indication */
    double lon;                 /**< Offset in longitude in minutes */
    azmuth_t lon_offset_dir;    /**< East / West indication */
    double alt;                 /**< Offset in altitude */
    datum_code_t datum;
} dtm_t;
#endif

#ifdef GBS
/**
 * @struct GBS
 *
 * This message outputs the results of the Receiver Autonomous Integrity Monitoring
 * Algorithm (RAIM).
 * The fields <b>errlat, errlon and erralt</b> output the standard deviation of the position
 * calculation, using all satellites which pass the RAIM test successfully.
 * The fields errlat, errlon and erralt are only output if the RAIM process passed
 * successfully (i.e. no or successful Edits happened). These fields are never output if 4 or
 * fewer satellites are used for the navigation calculation (because - in this case - integrity
 * can not be determined by the receiver autonomously)
 * The fields prob, bias and stdev are only output if at least one satellite failed in the
 * RAIM test. If more than one satellites fail the RAIM test, only the information for the
 * worst satellite is output in this message.
 *
 * $GPGBS,hhmmss.ss,errlat,errlon,erralt,svid,prob,bias,stddev*cs<CR><LF>
 * $GPGBS,235458.00,1.4,1.3,3.1,03,,-21.4,3.8*5B
 *
 * Where:
 *  GBS - Protocol header
 *  235458.00 - UTC Time, Time to which this RAIM sentence belongs(hhmmss.sss)
 *  1.4  - Expected error in latitude
 *  1.3  - Expected error in longitude
 *  3.1  - Expected error in altitude
 *  03   - Satellite ID of most likely failed satellite
 *  ,,   - Probability of missed detection
 *  -21.4- Estimate on most likely failed satellite (a priori residual)
 *  3.8  - Standard deviation of estimated bias
 *  *40  - Checksum
 */
typedef struct
{
    utc_time_t *fix_time;     /**< UTC Time */
    float lat_error;    /**< Error in latitude */
    float lon_error;    /**< Error in longitude */
    float alt_error;    /**< Error in altitude */
    uint8_t sat_id;    /**< Satellite ID of failed satellite */
    float prob_miss;    /**< Probablility of missed detection */
    double failed_est;  /**< Estimate on most likely faile sat */
    float std_deviation;/**< Standard deviation of estimated bias */
} gbs_t;
#endif

#ifdef GPQ
/**
 * @struct gpq_t
 *
 * Poll message
 *
 * $xxGPQ,sid*cs<CR><LF>
 * $EIGPQ,RMC*3A
 *
 * Where:
 *  GPQ - Protocol header
 *  RMC - Sentence identifier
 *  *3A - Checksum
 *
 */
typedef struct
{
    char id[4]; /**< Sentence identifier */
} gpq_t;
#endif

#ifdef GRS
/**
 * @struct grs_t
 *
 * GNSS Range Residuals
 *
 * This messages relates to associated GGA and GSA messages.
 * If less than 12 SVs are available, the remaining fields are output empty. If more than 12 SVs
 * are used, only the residuals of the first 12 SVs are output, in order to remain consistent
 * with the NMEA standard.
 *
 * $GPGRS,hhmmss.ss, mode {,residual}*cs<CR><LF>
 * $GPGRS,082632.00,1,0.54,0.83,1.00,1.02,-2.12,2.64,-0.71,-1.18,0.25,,,*70
 *
 * Where:
 *  GRS - Protocol header
 *  082632.00 - UTC Time, Time of associated position fix
 *  1   - Mode u-blox receivers will always output Mode 1 residuals
 *  0.54 - Range residuals for SVs used in navigation. The SV order matches the order from the GSA sentence.
 *  *70  - Checksum
 */
typedef struct
{
    utc_time_t *fix_time; /**< UTC Time */
    uint8_t mode;   /**< Mode of receiver */
    float range;    /**< Range residuals for SVs */
} grs_t;
#endif

#ifdef GST
/**
 * @struct GNSS Pseudo Range Error Statistics
 *
 * $GPGST,hhmmss.ss,range_rms,std_major,std_minor,hdg,std_lat,std_long,std_alt*cs<CR><LF>
 * $GPGST,082356.00,1.8,,,,1.7,1.3,2.2*7E
 *
 * Where:
 *  GST - Protocol header
 *  082356.00 - UTC Time, Time of associated position fix (hhmmss.sss)
 *  1.8 - RMS value of the standard deviation of the ranges
 *  ,, - Standard deviation of semi-major axis
 *  ,, - Standard deviation of semi-minor axis
 *  ,, - Orientation of semi-major axis
 *  1.7 - Standard deviation of latitude, error in meters
 *  1.3 - Standard deviation of longitude, error in meters
 *  2.2 - Standard deviation of altitude, error in meters
 *  *7E - Checksum
 */
typedef struct
{
    utc_time_t *fix_time;     /**< 082356.00 - UTC Time, Time of associated position fix (hhmmss.sss) */
    float rms;          /**< 1.8 - RMS value of the standard deviation of the ranges */
    float std_dev_maj;
    float std_dev_min;
    float orientation;
    float std_dev_lat;
    float std_dev_lon;
    float std_dev_alt;
} gst_t;
#endif



#ifdef THS
/**
 * @enum Valid Heading Codes
 */
typedef enum
{
    VEHICLE_UKNOWN = 0,
    VEHICLE_AUTONOMOUS,
    VEHICLE_ESTIMATED,
    VEHICLE_MANUAL,
    VEHICLE_SIMULATOR,
    VEHICLE_NOT_VALID
} vehicle_status_t;

/**
 * @struct Actual vehicle heading
 *
 * Actual vehicle heading in degrees, true heading.
 *
 * $GPTHS,headt,status*cs<CR><LF>
 * $GPTHS,77.52,E*32
 *
 * Where:
 *  THS - Protocol header
 *  77.52 - Heading of vehicle( true )
 *  E - Mode indicator: A = autonomous, E = Estimated (dead reckoning), M = Manual input, S = Simulator, V = Data not valid
 *  *32 - Checksum
 */
typedef struct
{
    double heading; /**< True heading */
    vehicle_status_t status;
} ths_t;
#endif

#ifdef TXT
typedef enum
{
    TXT_ERROR = 0,
    TXT_WARNING,
    TXT_NOTICE,
    TXT_USER = 7
} TXT_TYPE_t;
/**
 * @struct Text Transmission
 *
 * This message is not configured through CFG-MSG, but instead through CFG-INF.
 * This message outputs various information on the receiver, such as power-up screen,
 * software version etc. This message can be configured using UBX Protocol message
 *
 * $GPTXT,xx,yy,zz,ascii data*cs<CR><LF>
 * $GPTXT,01,01,02,u-blox ag - www.u-blox.com*50
 *
 * Where:
 *  TXT - Protocol header
 *  01 - Total number of messages in this transmission, 01..99
 *  01 - Message number in this transmission, range 01..xx
 *  02 - Text identifier, u-blox GPS receivers specify the severity of the message with this number.
 *     - 00 = ERROR
 *     - 01 = WARNING
 *     - 02 = NOTICE
 *     - 07 = USER
 *  www.ublox.com - Any ASCII text
 *  *67 - Checksum
 */
typedef struct
{
    uint8_t num_of_mesg;
    uint8_t mesg_num;
    TXT_TYPE_t mesg_type;
    char mesg[MAX_TXT_SIZE];
} txt_t;
#endif

#ifdef ZDA
/**
 * @struct Time and Date
 *
 * ZDA mainly shows the time and date. This message is included only with systems which support a
 * time-mark output pulse identified as “1PPS”. Output the time associated with the current 1PPS pulse.
 * Each message is output within a few hundred ms after the 1PPS pulse output and tells the time of the
 * pulse tha
 *
 * $GPZDA,061617.249,03,04,2013,,*59
 *
 * Where:
 *  061617.249 - UTC time( hhmmss.sss )
 *  03 - Day( dd )
 *  04 - Month( mm )
 *  2013 - Year ( yyyy )
 *  ,, - Local zone hours
 *  ,, - Local zone minutes
 *  *59 - Checksum
 */
typedef struct
{
    TimeStruct *time;
    uint8_t local_hour;
    uint8_t local_min;
} zda_t;

#endif
/******************************************************************************
* Variables
*******************************************************************************/


/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
} // extern "C"
#endif

#endif /*GPS_DEFS_H_*/

/*** End of File **************************************************************/