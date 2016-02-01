#ifndef _GPS_CONFIG_H
#define _GPS_CONFIG_H

#define UBLOX_6
//#define QUECTEL_L10
//#define QUECTEL_L30
//#define QUECTEL_L80
//#define HORNET_NANO

/* Universal sentences automatically supported
   GGA,RMC,GSA,GSV,GLL,VTG
*/

#if defined( UBLOX_6 )
#define DTM
#define GBS
#define GPQ
#define GRS
#define GST
#define THS
#define TXT
#endif

#if defined( QUECTEL_L10 )
#define ZDA
#define TXT
#endif

#if defined( QUECTEL_L30 )
#define ZDA
#endif

#if defined( QUECTEL_L80 )
#define TXT
#endif

#if defined( HORNET_NANO )
#define MSS

#endif

#endif
