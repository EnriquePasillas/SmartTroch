#ifndef MANAGER_TIMER_H
#define MANAGER_TIMER_H

#include "i2c_lsm6dsm.h"
#include "Init_Net.h"

#define FALLED_TIME (250)               // 0.25 second for timer repeate
#define TIMER_TIME (1000)
#define TIMER_TIME_BUTTON (300)
#define TIMER_TIME_ACTIVITY (350)

static wiced_timer_t timerHandle_reset;
static wiced_timer_t timerHandle_boton;

uint16_t count_down=0;
//wiced_bool_t down=WICED_FALSE;
static wiced_timed_event_t guardian;

void Time_reboot(void* arg);
void hc_protocol();
void wi_protocol();
static wiced_result_t guardian_v( void );
void network_re_up();


void init_all_timer(){
/*  Initialize timer*/
    wiced_rtos_init_timer(&timerHandle_reset, TIMER_TIME, Time_reboot, NULL);
    wiced_rtos_init_timer(&timerHandle_boton, TIMER_TIME_BUTTON, timer_button, NULL);
    wiced_rtos_init_timer(&wi_timer, FALLED_TIME, wi_protocol, NULL);
    wiced_rtos_init_timer(&hc_timer, TIMER_TIME_ACTIVITY, hc_protocol, NULL);

    wiced_rtos_start_timer(&timerHandle_reset);
    wiced_rtos_start_timer(&timerHandle_boton);
    wiced_rtos_start_timer(&wi_timer);
    wiced_rtos_start_timer(&hc_timer);


/*Evento complemntario de voltaje de entrada del cargador*/
    wiced_rtos_register_timed_event( &guardian, WICED_HARDWARE_IO_WORKER_THREAD, &guardian_v, 500, 0 );


//    wiced_rtos_register_timed_event( &guardian, WICED_HARDWARE_IO_WORKER_THREAD, &guardian_v, 500, 0 );



}

void Time_reboot(void* arg){
    wiced_bool_t upnet=WICED_FALSE;

     if(flag_tcp==3){
         upnet=wiced_network_is_up(WICED_STA_INTERFACE);
         if(upnet==WICED_FALSE){
                     WPRINT_APP_INFO( ("down %d \r\n",count_down) );

             count_down=count_down+1;
             if(count_down==3600){
                 wiced_framework_reboot();
             }
         }
         else{
             count_down=0;
         }
     }

}

void hc_protocol(){
    imu_read();
    //WPRINT_APP_INFO(("Gx: %d,\t Gy: %d\t, CONTADOR: %d\n\n", imu.gx, imu.gy, hc_counter));

    imu.ax_actual = (int)imu.ax;
    imu.ax_diff = imu.ax_actual - imu.ax_previous;
    imu.ax_previous = imu.ax_actual;
    imu.ay_actual = (int)imu.ay;
    imu.ay_diff = imu.ay_actual - imu.ay_previous;
    imu.ay_previous = imu.ay_actual;
    imu.az_actual = (int)imu.az;
    imu.az_diff = imu.az_actual - imu.az_previous;
    imu.az_previous = imu.az_actual;

    //WPRINT_APP_INFO(("Ax diff: %d ; Ay diff: %d ; Az diff: %d\n\n", imu.ax_diff, imu.ay_diff, imu.az_diff));
    if(flag_hc_is_active){
        if( (ACC_G_MAX < imu.ax_diff || imu.ax_diff < ACC_G_MIN) || (ACC_G_MAX < imu.ay_diff || imu.ay_diff < ACC_G_MIN) || (ACC_G_MAX < imu.az_diff || imu.az_diff < ACC_G_MIN) || (hc_gflag == WICED_TRUE) ){
            hc_gflag = WICED_TRUE;
            hc_counter ++;
            sprintf(hc_counter_print,"%d\r\n", hc_counter);
            if(hc_counter < imu_alert_count){
                wiced_uart_transmit_bytes(WICED_UART_1, HC_G_ALERT, sizeof(HC_G_ALERT)-1);
                wiced_uart_transmit_bytes(WICED_UART_1, HC_NO_MOVE, sizeof(HC_NO_MOVE)-1);
                wiced_uart_transmit_bytes(WICED_UART_1, hc_counter_print, strlen(hc_counter_print)-1);
            }
            if(hc_counter >= imu_alert_count){

                //CHECK IF GYROSCOPE HAVE ANY ANGLE HIGHER THAT
                if( (G_MAX < imu.gx || imu.gx < G_MIN) || (G_MAX < imu.gy || imu.gy < G_MIN) ){
                    wiced_uart_transmit_bytes(WICED_UART_1, HC_FALLED, sizeof(HC_FALLED)-1);
                    wiced_uart_transmit_bytes(WICED_UART_1, hc_counter_print, strlen(hc_counter_print)-1);

                    //CHECK IF ACCELEROMETER DON'T HAVE MOVEMENTES
                    if( (ACC_MIN < imu.ax_diff && imu.ax_diff < ACC_MAX) && (ACC_MIN < imu.ay_diff && imu.ay_diff < ACC_MAX) && (ACC_MIN < imu.az_diff && imu.az_diff < ACC_MAX) ){
                        if(hc_counter==(imu_alert_count*2)){
                            faslh_imu=WICED_TRUE;
                        }

                        if(hc_counter >= (imu_alert_count*2)){                               // Equivalent to 30 seconds
                            wiced_uart_transmit_bytes(WICED_UART_1, HC_WAKE_UP, sizeof(HC_WAKE_UP)-1);
                            if(hc_counter == (imu_alert_count*2+25)){                           // Equivalent to 45 seconds
                                wiced_rtos_stop_timer(&hc_timer);
                                hc_flag = WICED_TRUE;
                                GPIO_falledman=1;
                                wiced_uart_transmit_bytes(WICED_UART_1, HC_ALERT_HC, sizeof(HC_ALERT_HC)-1);
                            }
                        }
                    } else{hc_counter = 0; hc_gflag = WICED_FALSE; }
                } else{hc_counter = 0; hc_gflag = WICED_FALSE;}
            }
        }

    }


}

void wi_protocol(){
    imu_read();
    //WPRINT_APP_INFO(("%d\t%d\t%d\t%d\t%d\n", imu.gx, average_x, imu.gy, average_y, wi_counter));
//    wiced_gpio_output_high( WICED_LED1 );
    if(BLE_flag == WICED_TRUE){
//        wiced_uart_transmit_bytes(WICED_UART_1, CENTER, sizeof(CENTER)-1);

        if( (WIX_MIN < average_x && average_x < WIX_MAX) && (WIY_MIN < average_y && average_y < WIY_MAX) ){
//                wiced_gpio_output_high( WICED_LED1 );

                // ******* AQUI SE DESACTIVA EL WIFI ********
                wiced_uart_transmit_bytes(WICED_UART_3, BLE_CENTER_R, sizeof(BLE_CENTER_R)-1);


        }
        else{
//            wi_counter = 0;
        }
    }
}

static wiced_result_t guardian_v( void ){
    wiced_gpio_input_get_bool(VOLTAGE_LEVEL,&level);
    if((flag_vin==1)){
        wiced_gpio_input_get_bool(VOLTAGE_LEVEL,&level);
        switch(level){
           case 1:

            set_gpio_menssage(AYUDA,0);
            set_gpio_menssage(CONFIRMACION,0);
            set_gpio_menssage(EVACUACION,0);
            set_gpio_menssage(ENABLE_A1,0);
            set_gpio_menssage(FALLENMEN,0);
            set_gpio_menssage(INACTIVITY,0);
            Un_Set_config(2);
            wiced_framework_reboot();

               break;

           case 0:
            Un_Set_config(3);
            wiced_framework_reboot();
                break;

           default:
               break;
           }
    }

    wiced_gpio_input_get_bool(VOLTAGE_LEVEL,&level);
    if((level==1)){
        wiced_gpio_output_high(XRES_BLE);
        WPRINT_APP_INFO( ("xres high\r\n") );

    }
    else{
        count_xrest_bt=count_xrest_bt+1;

            wiced_uart_transmit_bytes( WICED_UART_3, WIFI_ONLINE, sizeof( WIFI_ONLINE ) - 1 );
            wiced_uart_transmit_bytes( WICED_UART_1, WIFI_ONLINE, sizeof( WIFI_ONLINE ) - 1 );

              if(count_xrest_bt==10){
                  xrest_bt();
                  WPRINT_APP_INFO( ("activate xres high\r\n") );
                  count_xrest_bt=0;
              }
    }

    if(charger_led==1){
        blink_stand_by();
    }

    return WICED_SUCCESS;
}


#endif  /* stdbool.h */
