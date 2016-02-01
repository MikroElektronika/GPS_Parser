/****************************************************************************
* Title                 :   GPS Parser
* Filename              :   gps_parser.h
* Origin Date           :   08/25/2015
* Notes                 :   None
*****************************************************************************/
/************************* CHANGE LIST **************************************
*
*    Date    Software Version    Initials   Description
*  08/25/2015    .0.1            RL         All supported strings implemented
*
*****************************************************************************/
/** @file gps_parser.h
 *  @brief Parses GPS strings as defined by the NMEA 0183 standards
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
 *  @mainpage GPS Parser
 *
 *  @section Intro
 *  Parsing multiple GPS strings in an application can add
 *  acuracy and depth to the application available data.  This
 *  library is a simple parser that suports multiple strings as
 *  defined in the NMEA 0183 standards.
 *
 *  @section Usage
 *  Configuration of the parser is found in the gps_config.h.
 *  Uncomment the type of GPS you are using or define your
 *  own using the availabe sentence types as defined there.
 *
 *  As data is incoming into the interface, feed each char to
 *  the <type>gps_put</type> function.  Then, in the application
 *  loop, call the <type>gps_parse</type> function.  The parse
 *  function will only parse if the buffer contains a valid sentence.
 *
 */

#ifndef GPS_PARSER_H_
#define GPS_PARSER_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include "time.h"
#include "gps_defs.h"

/******************************************************************************
* Preprocessor Constants
*******************************************************************************/


/******************************************************************************
* Configuration Constants
*******************************************************************************/


/******************************************************************************
* Macros
*******************************************************************************/



/******************************************************************************
* Typedefs
*******************************************************************************/


/******************************************************************************
* Variables
*******************************************************************************/


/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief gps_put
 *
 * Can be placed in either polling or interrupt based
 * rx.  Place each char from the feed with this function.
 *
 * @code
 * if( UART1_Data_Ready )
 *     gps_put( UART1_Read() );
 * @endcode
 *
 * @code
 * void UART3_RX_ISR() iv IVT_INT_USART3 ics ICS_AUTO
 * {
 *   if( RXNE_USART3_SR_bit )
 *   {
 *       gps_put_char( UART3_DR );
 *   }
 * }
 * @endcode
 *
 * @param input - individual char from GPS feed
 */
void gps_put( char input );

/**
 * @brief gps_parse
 *
 * Checks if a valid sentence exists on the buffer.  If
 * one exists then parses into respective data.
 */
void gps_parse();

/***************** Common ***************/
/**
 * @brief Current longitude
 *
 * @return location_t * - location with both degrees, minutes, and azmuth
 */
location_t* gps_current_lon( void );

/**
 * @brief Current latitude
 *
 * @return location_t * - location with both degrees, minutes, and azmuth
 */
location_t* gps_current_lat( void );

/**
 * @brief Current time
 *
 * @return TimeStruct * - UTC time in hours, minutes, seconds
 * as well as month, day, and year.
 */
TimeStruct* gps_current_time( void );

/**
 * @brief Current fix time
 *
 * @return utc_time_t * - Most current fix time in hours, minutes, seconds
 */
utc_time_t* gps_current_fix( void );

/****************** GGA ******************/
/**
 * @brief GGA sentence with quality of fix from
 * most recent fix
 *
 * @return fix_t - Fix quality
 * @retval INVALID = 0
 * @retval GPS_FIX = 1  ( SPS )
 * @retval DGPS_FIX = 2
 * @retval PPS_FIX = 3
 * @retval REAL_TIME_KINEMATIC = 4
 * @retval FLOAT_RTK = 5
 * @retval ESTIMATED = 6
 * @retval MANUAL_MODE = 7
 * @retval SIMULATION_MODE = 8
 */
fix_t gps_gga_fix_quality( void );

/**
 * @brief GGA sentence with number of sat fixes
 *
 * @return uint8_t
 * @retval 0 - number of supported channels
 */
uint8_t gps_gga_satcount( void );

/**
 * @brief GGA sentence horizontal dilution
 *
 * Geometric DOP is to state how errors in the measurement
 * will affect the final state estimation
 *
 * @return float
 */
float gps_gga_hor_dilution( void );

/**
 * @brief GGA sentence altitude dilution
 *
 * Geometric DOP is to state how errors in the measurement
 * will affect the final state estimation
 *
 * @return double
 */
double gps_gga_altitude( void );

/**
 * @brief GGA sentence altitude above mean seal level
 *
 * @return double
 */
double gps_gga_msl( void );

/**
 * @brief GGA sentence time since last update
 *
 * @return unint16_t
 */
uint16_t gps_gga_lastDGPS_update( void );

/**
 * @brief GGA sentence station ID of reporting station
 *
 * @return uint16_t
 */
uint16_t gps_gga_DGPS_stationID( void );

/***************** GLL ******************/
/**
 * @brief GLL sentence state of Loran
 *
 * @return ACTIVE_t
 * @retval LORAN_UNKNOWN = 0
 * @retval LORAN_ACTIVE = 1
 * @retval LORAN_VOID = 2
 */
ACTIVE_t gps_gll_active( void );

/***************** GSA ******************/
/**
 * @brief GSA sentence mode of sat
 *
 * @return GSA_MODE_t
 * @retval GSA_UNKNOWN = 0
 * @retval GSA_AUTO_MODE = 4
 * @retval GSA_MANUAL_MODE = 5
 */
GSA_MODE_t gps_gsa_mode( void );

/**
 * @brief GSA sentence fix type
 *
 * @return GSA_MODE_t
 * @retval GSA_UNKNOWN = 0
 * @retval GSA_NO_FIX = 1
 * @retval GSA_2D_FIX = 2
 * @retval GSA_3D_FIX = 3
 */
GSA_MODE_t gps_gsa_fix_type( void );

/**
 * @brief GSA sentence prns of reporting sats
 *
 * @return uint8_t - upto 12 prns
 */
uint8_t *gps_gsa_sat_prn( void );

/**
 * @brief GSA sentence dilution of precision
 *
 * Geometric DOP is to state how errors in the measurement
 * will affect the final state estimation
 *
 * @return float
 */
float gps_gsa_precision_dilution( void );

/**
 * @brief GSA sentence dilution of precision
 *
 * Geometric DOP is to state how errors in the measurement
 * will affect the final state estimation
 *
 * @return float
 */
float gps_gsa_horizontal_dilution( void );

/**
 * @brief GSA sentence dilution of precision
 *
 * Geometric DOP is to state how errors in the measurement
 * will affect the final state estimation
 *
 * @return float
 */
float gps_gsa_vertical_dilution( void );

/***************** GSV *****************/
// TODO: Must combine sat info and clear
/***************** RMC *****************/
/**
 * @brief RMC sentence status of sat
 *
 * @return GPS_STATUS_t
 * @retval RMC_UKNOWN = 0
 * @retval RMC_ACTIVE = 1
 * @retval RMC_VOID = 2
 * @retval RMC_AUTONOMOUS = 3
 * @retval RMC_DIFFERENTIAL = 4
 * @retval RMC_NOT_VALID = 5
 */
GPS_STATUS_t gps_rmc_status( void );

/**
 * @brief RMC sentence speed in knots
 *
 * @return double
 */
double gps_rmc_speed( void );

/**
 * @brief RMC sentence track angle in degrees
 *
 * @return double
 */
double gps_rmc_track( void );

/**
 * @brief RMC sentence magnetic variation
 *
 * @return double
 */
double gps_rmc_mag_var( void );

/**
 * @brief RMC sentence direction of magnetic variation
 *
 * @return azmuth_t
 * @retval UNKNOWN = 0
 * @retval NORTH = 1
 * @retval SOUTH = 2
 * @retval EAST = 3
 * @retval WEST = 4
 */
azmuth_t gps_rmc_direction( void );

/**
 * @brief RMC sentence mode
 *
 * @return GPS_STATUS_t
 * @retval RMC_UKNOWN = 0
 * @retval RMC_ACTIVE = 1
 * @retval RMC_VOID = 2
 * @retval RMC_AUTONOMOUS = 3
 * @retval RMC_DIFFERENTIAL = 4
 * @retval RMC_NOT_VALID = 5
 */
GPS_STATUS_t gps_rmc_mode( void );

/**************** VTG *****************/
/**
 * @brief VTG sentence track
 *
 * @return double
 */
double gps_vtg_track( void );

/**
 * @brief VTG sentence magnetic track
 *
 * @return double
 */
double gps_vtg_mag( void );

/**
 * @brief VTG sentence speed in knots
 *
 * @return double
 */
double gps_vtg_speedknt( void );

/**
 * @brief VTG sentence speed km
 *
 * @return double
 */
double gps_vtg_speedkm( void );

#ifdef DTM
/**
 * @brief DTM sentence local datum
 *
 * @return datum_code_t
 * @retval DATUM_UNKNOWN = 0
 * @retval WGS84 = 1
 * @retval WGS72 = 2
 * @retval SGS85 = 3
 * @retval PE90 = 4
 * @retval USER_DEFINED = 5
 * @retval IHO = 6
 */
datum_code_t gps_dtm_local( void );

/**
 * @brief DTM sentence local datum subdivision code
 *
 * @return char *
 */
char *gps_dtm_localoffset( void );

/**
 * @brief DTM sentence latitude offset in minutes
 *
 * @return double
 */
double gps_dtm_latoffset( void );

/**
 * @brief DTM sentence azmuth of latitude offset
 *
 * @return azmuth_t
 * @retval UNKNOWN = 0
 * @retval NORTH = 1
 * @retval SOUTH = 2
 * @retval EAST = 3
 * @retval WEST = 4
 */
azmuth_t gps_dtm_lat_offset_dir( void );

/**
 * @brief DTM sentence longitude offset
 *
 * @return double
 */
double gps_dtm_lonoffset( void );

/**
 * @brief DTM sentence azmuth of longitude offset
 *
 * @return azmuth_t
 * @retval UNKNOWN = 0
 * @retval NORTH = 1
 * @retval SOUTH = 2
 * @retval EAST = 3
 * @retval WEST = 4
 */
azmuth_t gps_dtm_lon_offset_dir( void );

/**
 * @brief DTM sentence altitude offset
 *
 * @return double
 */
double gps_dtm_altoffset( void );

/**
 * @brief DTM sentence datum
 *
 * @return datum_code_t
 * @retval DATUM_UNKNOWN = 0
 * @retval WGS84 = 1
 * @retval WGS72 = 2
 * @retval SGS85 = 3
 * @retval PE90 = 4
 * @retval USER_DEFINED = 5
 * @retval IHO = 6
 */
datum_code_t gps_dtm_datum( void );
#endif

#ifdef GBS
/**
 * @brief GBS sentence latitude error
 *
 * @return float
 */
float gps_gbs_laterror( void );

/**
 * @brief GBS sentence longitude
 *
 * @return float
 */
float gps_gbs_lonerror( void );

/**
 * @brief GBS altitude error
 *
 * @return float
 */
float gps_gbs_alterror( void );

/**
 * @brief GBS sentence satellite id of failed satellite
 *
 * @return uint8_t
 */
uint8_t gps_gbs_satid( void );

/**
 * @brief GBS sentence probability of missed detection
 *
 * @return float
 */
float gps_gbs_probmiss( void );

/**
 * @brief GBS sentence Estimate on most likely failed satellite
 *
 * @return double
 */
double gps_gbs_failedest( void );

/**
 * @brief GBS sentence standard deviation of estimate
 *
 * @return float
 */
float gps_gbs_std_deviation( void );
#endif

#ifdef GPQ
/**
 * @brief GPQ sentence identifier
 *
 * @return char * - 2 chars
 */
char *gps_gpq_message( void );
#endif

#ifdef GRS
/**
 * @brief GRS sentence mode
 *
 * Mode u-blox receivers will always output Mode 1
 * residuals
 *
 * @return uint8_t
 */
uint8_t gps_grs_mode( void );

/**
 * @brief GRS sentence range of residuals
 *
 * @return float
 */
float gps_grs_range( void );
#endif

#ifdef GST
/**
 * @brief GST sentence rms value of standard
 * deviation
 *
 * @return float
 */
float gps_gst_rms( void );

/**
 * @brief GST sentence standard deviation major axis
 *
 * @return float
 */
float gps_gst_stddev_major( void );

/**
 * @brief GST sentence standard deviation of minor axis
 *
 * @return float
 */
float gps_gst_stddev_minor( void );

/**
 * @brief GST sentence orientation of semi-major axis
 *
 * @return float
 */
float gps_gst_orientation( void );

/**
 * @brief GST sentence standard deviation of latitude
 *
 * @return float
 */
float gps_gst_stddev_lat( void );

/**
 * @brief GST sentence standard deviation of longitude
 *
 * @return float
 */
float gps_gst_stddev_lon( void );

/**
 * @brief GST sentence standard deviation of altitude
 *
 * @return float
 */
float gps_gst_stddev_alt( void );
#endif

#ifdef THS
/**
 * @brief THS sentence heading of vehicle
 *
 * @return double
 */
double gps_ths_heading( void );

/**
 * @brief THS sentence status of vehicle mode
 *
 * @return vehicle_status_t
 * @retval VEHICLE_UKNOWN = 0
 * @retval VEHICLE_AUTONOMOUS = 1
 * @retval VEHICLE_ESTIMATED = 2
 * @retval VEHICLE_MANUAL = 3
 * @retval VEHICLE_SIMULATOR = 4
 * @retval VEHICLE_NOT_VALID = 5
 */
vehicle_status_t gps_ths_status( void );
#endif

#ifdef TXT
// TODO:

#endif

#ifdef ZDA
/**
 * @brief ZDA sentence get local hour
 *
 * @return uint8_t
 */
uint8_t gps_zda_local_hour( void );

/**
 * @brief ZDA sentence get local minute
 *
 * @return uint8_t
 */
uint8_t gps_zda_local_min( void );
#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif /*GPS_PARSER_H_*/

/*** End of File **************************************************************/