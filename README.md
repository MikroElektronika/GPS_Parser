> ![MikroE](http://www.mikroe.com/img/designs/beta/logo_small.png)
> #[GPS click](http://www.mikroe.com/click/gps/)#
> #[GPS2 click](http://www.mikroe.com/click/gps2/)#
> #[GPS3 click](http://www.mikroe.com/click/gps3/)#
> #[Nano GPS click](http://www.mikroe.com/click/nano-gps/)#
> ##By [MikroElektronika](http://www.mikroe.com)
---

## Installation
Use the [package manager](http://www.mikroe.com/package-manager/) to install the library for your architecture.

###Example on STM32
```
#include "scheduler.h"
#include "gps_parser.h"

#define MSG( TXT ) UART1_Write_Text( TXT )

sbit STATUS_LED at GPIOB_ODR.B0;
sbit RX_LED at GPIOB_ODR.B1;
sbit POWER at GPIOA_ODR.B0;
sbit RESET at GPIOC_ODR.B2;
sbit WAKEUP at GPIOA_ODR.B4;

void heartbeat()
{
    STATUS_LED = ~STATUS_LED;
}


void check_gps()
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
void init_timer2()
{
    RCC_APB1ENR.TIM2EN = 1;
    TIM2_CR1.CEN = 0;
    TIM2_PSC = 575;
    TIM2_ARR = 62499;
    TIM2_DIER.UIE = 1;
    TIM2_CR1.CEN = 1;
}

void gps_initialize()
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

void system_init()
{
    GPIO_Digital_Output( &GPIOA_BASE, _GPIO_PINMASK_0 );
    GPIO_Digital_Output( &GPIOB_BASE, _GPIO_PINMASK_0 | _GPIO_PINMASK_1 );
    GPIO_Digital_Output( &GPIOC_BASE, _GPIO_PINMASK_2 );

    GPIO_Digital_Input( &GPIOA_BASE, _GPIO_PINMASK_4 );

    UART1_Init_Advanced( 115200, _UART_8_BIT_DATA, _UART_NOPARITY,
                         _UART_ONE_STOPBIT, &_GPIO_MODULE_USART1_PA9_10 );
    Delay_ms( 100 );
    RXNEIE_USART1_CR1_bit = 1;

    UART2_Init_Advanced(  9600, _UART_8_BIT_DATA, _UART_NOPARITY,
                          _UART_ONE_STOPBIT, &_GPIO_MODULE_USART2_PD56 );

    Delay_ms( 100 );
    RXNEIE_USART2_CR1_bit = 1;

    UART3_Init_Advanced( 9600, _UART_8_BIT_DATA, _UART_NOPARITY,
                         _UART_ONE_STOPBIT, &_GPIO_MODULE_USART3_PD89 );

    Delay_ms( 100 );
    //RXNEIE_USART3_CR1_bit = 1;

    init_timer2();
    task_scheduler_init( 500 );
    //gps_initialize();

    NVIC_IntEnable( IVT_INT_USART1 );
    NVIC_IntEnable( IVT_INT_USART2 );
    //NVIC_IntEnable( IVT_INT_USART3 );
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

void UART1_RX_ISR() iv IVT_INT_USART1 ics ICS_AUTO
{
    if( RXNE_USART1_SR_bit )
    {
        UART1_Write( USART1_DR );
        RX_LED = ~RX_LED;
    }
}


void UART2_RX_ISR() iv IVT_INT_USART2 ics ICS_AUTO
{
    if( RXNE_USART2_SR_bit )
    {
        char tmp = USART2_DR;
        gps_put( tmp );
        //UART1_Write( tmp );
        RX_LED = ~RX_LED;
    }
}


void UART3_RX_ISR() iv IVT_INT_USART3 ics ICS_AUTO
{
    if( RXNE_USART3_SR_bit )
    {
        char tmp = USART3_DR;
        gps_put( tmp );
        RX_LED = ~RX_LED;
    }
}

void Timer2_interrupt() iv IVT_INT_TIM2
{
    TIM2_SR.UIF = 0;
    task_scheduler_clock();
}
```