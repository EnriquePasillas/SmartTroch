#include "wiced.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "Starter.h"
#include "Init_Net.h"


/*
 *  * WICED Join Options *
    #define WICED_JOIN_RETRY_ATTEMPTS             (500)
 *
 * */

//wiced_bool_t
void application_start( ){
    wiced_init();
    system_starter();

/*  Set preferred association band */
    wwd_wifi_set_preferred_association_band(WLC_BAND_2G );

    wiced_gpio_input_get_bool(VOLTAGE_LEVEL,&level);
    flag_charger=is_charger();

    imu_alert_count=20;
    wiced_gpio_input_get_bool(READ_BT,&center_ping);
//    wiced_rtos_register_timed_event( &re_connect, WICED_NETWORKING_WORKER_THREAD, &network_re_up, 1500, 0 );


    Set_SSID("-SSF-HARDWARE",18,WICED_UART_1);
    Set_KEY("-KLasec123.",18,WICED_UART_1);
    Set_SERVER("-I10.86.10.1",18,WICED_UART_1);
    Set_MASK("-M255.255.255.0",18,WICED_UART_1);
    Set_IP("-C10.86.10.115",18,WICED_UART_1);
    Set_GATEWAY("-G10.86.10.254",18,WICED_UART_1);
    Set_config();

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

}





