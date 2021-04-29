#include "wiced.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "dct_data_write.h"
#include "manager_net.h"
#include "manager_gpio.h"
#include "manager_thread.h"
#include "manager_dct.h"
#include "manager_tcp_client.h"
#include  "manager_adc.h"

#include  "manager_menssage.h"

//#include    "manager_bt.h"
#include    "i2c_lsm6dsm.h"
#include    "wireless_config.h"
#include    "Init_net.h"


/*MODIFICAR EL NUMERO DE INTENTOS  PARA EL CODIGO QUE SE REINICIA*/


/******************************************************
 *                    Timer/ counts
 ******************************************************/
#define FALLED_TIME (250)               // 0.25 second for timer repeate
#define TIMER_TIME (1000)
#define TIMER_TIME_BUTTON (300)
#define TIMER_TIME_ACTIVITY (350)


uint8_t count_down=0;
wiced_bool_t down=WICED_FALSE;

static wiced_timer_t timerHandle_reset;
static wiced_timer_t timerHandle_boton;


/****************************************************
 *         Static Function Declarations
 ****************************************************/


void TIME(void* arg);
void timer_button(void* arg);




void application_start( )
{
    /* Enable Wi-Fi powersave */
    #ifdef USE_POWERSAVE_POLL
        wiced_wifi_enable_powersave();
    #else
        const uint8_t return_to_sleep_delay = 10;
        wiced_wifi_enable_powersave_with_throughput( return_to_sleep_delay );
    #endif


/*  Initialize the device and WICED framework */
    wiced_init();
//    uart_init();
    init_tcp_w();
    imu_init();
    set_gpio();
    is_Access();
    is_config();    /*  check if the values networking is set */
    reset_sequence();   /* toggleo rapido de leds */


/* Initialize the queue */
      wiced_rtos_init_queue(&queueHandle1, "uart_1", MESSAGE_SIZE, QUEUE_SIZE); /* Setup the semaphore which will be set by the button interrupt */
//      wiced_rtos_init_queue(&queueHandle2, "uart_2", MESSAGE_SIZE, QUEUE_SIZE); /* Setup the semaphore which will be set by the button interrupt */
      wiced_rtos_init_queue(&queueHandle3, "uart_3", MESSAGE_SIZE, QUEUE_SIZE); /* Setup the semaphore which will be set by the button interrupt */

      /* Initialize and start UART_1 thread */
      wiced_rtos_create_thread(&uart1_ThreadHandle, THREAD_PRIORITY, "uart_1", uart1_thread, THREAD_STACK_SIZE, NULL);
//      wiced_rtos_create_thread(&uart2_ThreadHandle, THREAD_PRIORITY, "uart_2", uart2_thread, THREAD_STACK_SIZE, NULL);
      wiced_rtos_create_thread(&uart3_ThreadHandle, THREAD_PRIORITY, "uart_3", uart3_thread, THREAD_STACK_SIZE, NULL);

      wiced_rtos_create_thread(&uart_main1_ThreadHandle, THREAD_PRIORITY, "main_uart1", uart_main1, THREAD_STACK_SIZE, NULL);
//      wiced_rtos_create_thread(&uart_main2_ThreadHandle, THREAD_PRIORITY, "main_uart2", uart_main2, THREAD_STACK_SIZE, NULL);
      wiced_rtos_create_thread(&uart_main3_ThreadHandle, THREAD_PRIORITY, "main_uart3", uart_main3, THREAD_STACK_SIZE, NULL);


/*  Initialize adc*/
    /* Initialise adc data. adc data are shared among multiple threads; therefore a mutex is required */
    memset( &adc_data, 0, sizeof( adc_data ) );
    wiced_rtos_init_mutex( &adc_data.mutex );

/* Setup a timed event that will take a measurement */
    wiced_rtos_register_timed_event( &adc_timed_event, WICED_HARDWARE_IO_WORKER_THREAD, take_adc_reading, 1 * SECONDS, 0 );

/*adc start*/
    wiced_adc_start();

/*  Get value gpios*/
    GPIO_Recib=get_gpio_menssage(CONFIRMACION);
    GPIO_Aux=get_gpio_menssage(AYUDA);
    GPIO_Evac=get_gpio_menssage(EVACUACION);
    EnableA1=get_gpio_menssage(ENABLE_A1);
    flag_hc_is_active=get_gpio_menssage(INACTIVITY);
    GPIO_activate=get_gpio_menssage(INACTIVITY);
    GPIO_falledman=get_gpio_menssage(FALLENMEN);

/*  Initialize timer*/

    wiced_rtos_init_timer(&timerHandle_reset, TIMER_TIME, TIME, NULL);
    wiced_rtos_init_timer(&timerHandle_boton, TIMER_TIME_BUTTON, timer_button, NULL);
    wiced_rtos_start_timer(&timerHandle_reset);
    wiced_rtos_start_timer(&timerHandle_boton);

//    wiced_platform_mcu_enable_powersave();

    wiced_rtos_init_timer(&wi_timer, FALLED_TIME, wi_protocol, NULL);
         wiced_rtos_start_timer(&wi_timer);
         wiced_rtos_init_timer(&hc_timer, TIMER_TIME_ACTIVITY, hc_protocol, NULL);
         wiced_rtos_start_timer(&hc_timer);


/*  Start mcu values  */
         memset(mcu_adc,'0',5);

        memset(mcu_gpio,'0',17);


/*  Set preferred association band */
        wwd_wifi_set_preferred_association_band(WLC_BAND_2G );

/*Interrupcion del boton de la lampara*/
        wiced_gpio_input_irq_enable(HELP_ACCUSE, IRQ_TRIGGER_FALLING_EDGE, btn_lamp_isr, NULL); /* Setup interrupt */

/*Interrupcion para indicar la entrada del cargador*/
        wiced_gpio_input_irq_enable(VOLTAGE_LEVEL, IRQ_TRIGGER_BOTH_EDGES, divisor_isr, NULL); /* Setup interrupt */

/*Evento complemntario de voltaje de entrada del cargador*/
        wiced_rtos_register_timed_event( &guardian, WICED_HARDWARE_IO_WORKER_THREAD, &guardian_v, 500, 0 );

/*Bandera de evento de cargador  */
        wiced_gpio_input_get_bool(VOLTAGE_LEVEL,&level);
        flag_charger=is_charger();

        get_mac_bt();
        Set_SSID("-SSF-DEMO",13,WICED_UART_1);
        Set_KEY("-KD3_lasec2020.,",17,WICED_UART_1);
        Set_SERVER("-I10.174.111.43",12,WICED_UART_1);
        Set_MASK("-M255.255.248.0",18,WICED_UART_1);
        Set_IP("-C10.174.111.200",18,WICED_UART_1);
        Set_GATEWAY("-G10.174.107.30",14,WICED_UART_1);
        Set_config();
        imu_alert_count=20;
        wiced_gpio_input_get_bool(READ_BT,&center_ping);

        if(center_ping==0){

             switch(flag_tcp){
                case 1:
                    break;

                case 0:
                    if((level==0)){
                        blink_cfg();
                        net_config();   /*Inicia la comuncicacion tcp a red de cpnfiguracion y asginacion de la lamapra*/
                    }
                    break;

                case 2:
                    if((level==1)){
                        charger_led=1;
                    }
                    break;
                case 3:
                    net_lamp();     /*Inicia la comuncicacion tcp a red asiganda*/
                    break;

                default:
                    break;
                }
        }

//    memset(MAC_BT_FACTORY,'\0',20 );
//    flag_hc_is_active=WICED_TRUE;


}


void TIME(void* arg){
    wiced_bool_t upnet=WICED_FALSE;
    if(flag_tcp==3){
        upnet=wiced_network_is_up(WICED_STA_INTERFACE);
        if(upnet==WICED_FALSE){
                 WPRINT_APP_INFO( ("down\r\n") );

         count_down=count_down+1;
         if(count_down==40){
             wiced_framework_reboot();
         }
        }
        else{
         count_down=0;
        }
    }



}







