/*******************************************************************************
* Title                 :   GPS Parser Demo
* Filename              :   GPS_click.c
* Author                :   RBL
* Origin Date           :   09/03/2015
* Notes                 :   None
*******************************************************************************/
/*************** MODULE REVISION LOG ******************************************
*
*    Date    Software Version    Initials   Description 
*  09/03/15         .1           RBL        Module Created.
*
*******************************************************************************/
/** 
 * @file GPS_click.c
 * @brief Example to the GPS Parser
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "scheduler.h"
#include "gps_parser.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/


/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/
#define MSG( TXT ) UART1_Write_Text( TXT )

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
sbit STATUS_LED at GPIOB_ODR.B0;
sbit POWER at GPIOA_ODR.B0;
sbit RESET at GPIOC_ODR.B2;
sbit WAKEUP at GPIOA_ODR.B4;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
// Used for visual confirmation of system running
static void heartbeat( void );
// Checks for valid gps messages
static void check_gps( void );
// Timer for scheduler
static void init_timer2( void );
// Initialize Nano GPS
static void gps_initialize( void );
// Initialize system
static void system_init( void );

/******************************************************************************
* Function Definitions
*******************************************************************************/
static void heartbeat()
{
    STATUS_LED = ~STATUS_LED;
}


static void check_gps()
{
    char text[80];
    location_t *me;

    me = gps_current_lat();

    sprintf( text, "Latitude\r\n\tDegrees: %d\r\n\tMinutes: %4f\r\n\tDirection %d\r\n",
             me->degrees, me->minutes, me->azmuth );
    MSG( text );

    me = gps_current_lon();

    sprintf( text, "Longitude\r\n\tDegrees: %d\r\n\tMinutes: %4f\r\n\tDirection %d\r\n",
             me->degrees, me->minutes, me->azmuth );
    MSG( text );

    sprintf( text, "Speed:\r\n\t%f\r\n", gps_rmc_speed() );
    MSG( text );

    sprintf( text, "Num of sats in view:\r\n\t%d\r\n", gps_gga_satcount() );
    MSG( text );

    sprintf( text, "Magnetic var:\r\n\t%f\r\n", gps_vtg_mag() );
    MSG( text );

    sprintf( text, "Tracking:\r\n\t%f\r\n", gps_rmc_track() );
    MSG( text );

    sprintf( text, "Altitude:\r\n\t%f\r\n", gps_gga_altitude() );
    MSG( text );

}

//Timer2 Prescaler :575; Preload = 62499; Actual Interrupt Time = 500 ms
static void init_timer2()
{
    RCC_APB1ENR.TIM2EN = 1;
    TIM2_CR1.CEN = 0;
    TIM2_PSC = 575;
    TIM2_ARR = 62499;
    TIM2_DIER.UIE = 1;
    TIM2_CR1.CEN = 1;
}

static void gps_initialize()
{
    RESET = 1;
    Delay_ms( 300 );
    RESET = 0;

    POWER = 0;
    Delay_ms( 100 );
    POWER = 1;
    Delay_ms( 2000 );
    POWER = 0;
    Delay_ms( 100 );
}

static void system_init()
{
    GPIO_Digital_Output( &GPIOA_BASE, _GPIO_PINMASK_0 );
    GPIO_Digital_Output( &GPIOB_BASE, _GPIO_PINMASK_0 | _GPIO_PINMASK_1 );
    GPIO_Digital_Output( &GPIOC_BASE, _GPIO_PINMASK_2 );

    GPIO_Digital_Input( &GPIOA_BASE, _GPIO_PINMASK_4 );

    UART1_Init_Advanced( 115200, _UART_8_BIT_DATA, _UART_NOPARITY,
                         _UART_ONE_STOPBIT, &_GPIO_MODULE_USART1_PA9_10 );
    RXNEIE_USART1_CR1_bit = 1;

    UART2_Init_Advanced(  9600, _UART_8_BIT_DATA, _UART_NOPARITY,
                          _UART_ONE_STOPBIT, &_GPIO_MODULE_USART2_PD56 );

    RXNEIE_USART2_CR1_bit = 1;
    Delay_ms( 100 );
        
    init_timer2();
    task_scheduler_init( 500 );
    gps_initialize();

    NVIC_IntEnable( IVT_INT_USART2 );
    NVIC_IntEnable(IVT_INT_TIM2);

    EnableInterrupts();
}

void main()
{
    system_init();
    task_add( check_gps, SCH_SECONDS_5 );
    task_add( heartbeat, SCH_SECONDS_1 );

    task_scheduler_start();

    while( 1 )
    {
        gps_parse();
        task_dispatch();
    }
}

void UART2_RX_ISR() iv IVT_INT_USART2 ics ICS_AUTO
{
    if( RXNE_USART2_SR_bit )
    {
        char tmp = USART2_DR;
        gps_put( tmp );
        RX_LED = ~RX_LED;
    }
}

void Timer2_interrupt() iv IVT_INT_TIM2
{
    TIM2_SR.UIF = 0;
    task_scheduler_clock();
}


/*************** END OF FUNCTIONS ***************************************************************************/
