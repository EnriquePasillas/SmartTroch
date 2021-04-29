#ifndef _MANAGER_GPIO_H
#define _MANAGER_GPIO_H
#include "stdbool.h"
#include "wiced.h"
#include "platform_peripheral.h"
#include "platform_init.h"

#include    "manager_dct.h"
#include    "manager_menssage.h"
#include    "dct_data_write.h"

//#include  "manager_adc.h"



#define XRES_BLE            WICED_GPIO_2      //gpio1
#define BOOT MODE           PIN_GPIO_7      //gpio7
#define VOLTAGE_LEVEL       WICED_GPIO_5      //gpio9
#define WIFI_STATUS         WICED_GPIO_7     //gpio11
#define UART_CTS            PIN_GPIO_13     //gpio13


#define STAT1               WICED_GPIO_1      //gpio0
#define STAT2               WICED_GPIO_6     //gpio10
#define HELP_ACCUSE         WICED_GPIO_8     //gpio12
#define TRIGGER             WICED_GPIO_10     //gpio14
#define READ_WIFI           WICED_GPIO_11     //gpio15
#define READ_BT             WICED_GPIO_12     //gpio16


#define HELP_CONFIRM        WICED_GPIO_17   //pwm0WICED_GPIO_13
#define available1          WICED_GPIO_14   //pwm1
#define OTA_LED             WICED_GPIO_16   //pwm3

#define WIFI_BLINK  (50)
#define TRIG_TIME   (25)

wiced_bool_t flag_ligth=0;
wiced_bool_t level;
wiced_bool_t center_ping;
wiced_bool_t BTN_LAMP;
wiced_bool_t B_STAT2=0;
wiced_bool_t B_STAT1=0;
int EnableA1 = 0;
wiced_bool_t isr_time_enable=WICED_FALSE;
wiced_bool_t flag_end_toggle=WICED_FALSE;

int count_isr=0;

uint8_t charger=0;
uint8_t charger_led=0;



uint8_t GPIO_Evac = 0;
uint8_t GPIO_Aux  = 0;
uint8_t GPIO_Recib = 0;
uint8_t GPIO_falledman=0;
uint8_t GPIO_activate=0;



char    mcu_adc[5];
unsigned char    mcu_gpio[17];
char    mcu[17];
//wiced_bool_t  is_imu_active=WICED_FALSE;


uint8_t count_xrest_bt=0;


wiced_result_t set_gpio();
void print_gpio_value( int index, wiced_gpio_t gpio );
void strong_ligth_sequence();
void sweet_ligth_sequence();
void next_ligth_sequence();
void start_toggle_sequence();
void identifique();
void reset_sequence();
void next_step(uint8_t stat );
void divisor_isr(void* arg);
void btn_lamp_isr(void* arg);
void timer_button(void* arg);

wiced_result_t set_gpio(){

    /*  Inputs  */

    //  Divsor de voltaje
    wiced_gpio_init( VOLTAGE_LEVEL, INPUT_PULL_DOWN );
    //  Status 1
    wiced_gpio_init( STAT1, INPUT_PULL_DOWN );
    //  Status 2
    wiced_gpio_init( STAT2, INPUT_PULL_DOWN );
    //  Boton de lamapra
    wiced_gpio_init( HELP_ACCUSE, INPUT_PULL_DOWN );
    //  Lectura constante con blue para evitar el bloqueo
    wiced_gpio_init( READ_BT, INPUT_PULL_DOWN );



    /*  Outputs */
    // Pin de control de flujo
    wiced_gpio_init( UART_CTS, OUTPUT_OPEN_DRAIN_PULL_UP );
    // Pin de reset bluetooth
    wiced_gpio_init( XRES_BLE, OUTPUT_OPEN_DRAIN_PULL_UP );
    // Estado de wifi
    wiced_gpio_init( WIFI_STATUS, OUTPUT_OPEN_DRAIN_PULL_UP );
    // Confirmacion de ayuda
    wiced_gpio_init(HELP_CONFIRM , OUTPUT_OPEN_DRAIN_PULL_UP );
    // Envio constante del estado de wifi para evitar el bloqueo
    wiced_gpio_init( READ_WIFI, OUTPUT_OPEN_DRAIN_PULL_UP );
    // Boton de disparo
    wiced_gpio_init( TRIGGER, OUTPUT_OPEN_DRAIN_PULL_UP );
    // Indacator led
    wiced_gpio_init( OTA_LED, OUTPUT_OPEN_DRAIN_PULL_UP );

    wiced_gpio_output_low(OTA_LED);
    wiced_gpio_output_low(TRIGGER);
    wiced_gpio_output_low(WIFI_STATUS);
    wiced_gpio_output_low(HELP_CONFIRM);
    wiced_gpio_output_low(XRES_BLE);


    return WICED_SUCCESS;
}

void print_gpio_value( int index, wiced_gpio_t gpio )
{
    wiced_bool_t    value = 0;
    if (wiced_gpio_input_get_bool( gpio, &value ) != WICED_SUCCESS)
    {
        printf(" WICED_BUTTON%d ( GPIO %d):Error", index, gpio);
    }
    else
    {
        printf(" WICED_BUTTON%d (GPIO %d):%d", index, gpio, value);
    }
}

void strong_ligth_sequence(){
    wiced_gpio_output_high(TRIGGER);
    wiced_rtos_delay_milliseconds( TRIG_TIME );
    wiced_gpio_output_low(TRIGGER);
    wiced_rtos_delay_milliseconds( TRIG_TIME );
    wiced_gpio_output_high(TRIGGER);
    wiced_rtos_delay_milliseconds( TRIG_TIME );
    wiced_gpio_output_low(TRIGGER);
    wiced_uart_transmit_bytes( WICED_UART_1, LIGTH_SWEET, sizeof( LIGTH_SWEET) - 1 );

    wiced_rtos_delay_milliseconds( TRIG_TIME );
}
void sweet_ligth_sequence(){
    wiced_gpio_output_high(TRIGGER);
    wiced_rtos_delay_milliseconds( TRIG_TIME );
    wiced_gpio_output_low(TRIGGER);

    wiced_uart_transmit_bytes( WICED_UART_1, LIGTH_STRONG, sizeof( LIGTH_STRONG ) - 1 );

    wiced_rtos_delay_milliseconds( TRIG_TIME );
}
void next_ligth_sequence(){
    wiced_gpio_output_high(TRIGGER);
    wiced_rtos_delay_milliseconds( TRIG_TIME );
    wiced_gpio_output_low(TRIGGER);
    wiced_rtos_delay_milliseconds( TRIG_TIME );
}

void start_toggle_sequence(){
//    wiced_gpio_output_high(TRIGGER);
//    wiced_rtos_delay_milliseconds( 250 );
    wiced_gpio_output_high(TRIGGER);
    wiced_rtos_delay_milliseconds( 3500 );
    wiced_gpio_output_low(TRIGGER);
    wiced_uart_transmit_bytes( WICED_UART_1, LIGTH_SEQUENCE, sizeof( LIGTH_SEQUENCE ) - 1 );
    wiced_rtos_delay_milliseconds( TRIGGER );
    flag_ligth=WICED_TRUE;
}

void identifique(){
    // Boton de disparo

    wiced_gpio_init( TRIGGER, OUTPUT_OPEN_DRAIN_PULL_UP );

    wiced_gpio_input_get_bool(STAT1,&B_STAT1);
    wiced_gpio_input_get_bool(STAT2,&B_STAT2);
    if((B_STAT1==WICED_FALSE)&&(B_STAT2==WICED_FALSE)){
        start_toggle_sequence();
    }
    else{
        next_ligth_sequence();

       if(B_STAT1==WICED_TRUE){

            strong_ligth_sequence();

        }
        else if(B_STAT2==WICED_TRUE){

            sweet_ligth_sequence();

        }

        wiced_gpio_input_get_bool(STAT1,&B_STAT1);
        wiced_gpio_input_get_bool(STAT2,&B_STAT2);

        if((B_STAT1==WICED_FALSE)&&(B_STAT2==WICED_FALSE)){
//            wiced_gpio_output_low(TRIGGER);
//            wiced_rtos_delay_milliseconds( 500 );
                wiced_gpio_output_low(TRIGGER);
                wiced_rtos_delay_milliseconds( 250 );
            wiced_uart_transmit_bytes( WICED_UART_1, LIGTH_NOT, sizeof( LIGTH_NOT ) - 1 );

            start_toggle_sequence();
        }

    }

    flag_end_toggle=WICED_FALSE;

}

void next_step(uint8_t stat ){
    if(stat==1){
        Set_flag_charger(1);


    }
    else if(stat==0){
        Set_flag_charger(0);

    }
}

void xrest_bt(){
    wiced_gpio_output_high(XRES_BLE);
    wiced_rtos_delay_milliseconds( 250 );
    wiced_gpio_output_low(XRES_BLE);
    wiced_rtos_delay_milliseconds( 250 );


}
void divisor_isr(void* arg)
{
    flag_vin=1;
}



void btn_lamp_isr(void* arg){
    wiced_gpio_input_get_bool(HELP_ACCUSE,&BTN_LAMP);
    if((EnableA1==1)&&(flag_ligth==WICED_TRUE)){
        wiced_gpio_input_get_bool(HELP_ACCUSE,&BTN_LAMP);
        if((BTN_LAMP==WICED_TRUE)&&(flag_ligth==WICED_TRUE)){
            GPIO_Recib=1;
            set_gpio_menssage(CONFIRMACION,GPIO_Recib);
            wiced_gpio_output_low(HELP_CONFIRM);
            wiced_uart_transmit_bytes( WICED_UART_1, BTN_EVENT, sizeof(BTN_EVENT)-1 );
            flag_ligth=WICED_FALSE;
        }

    }
    else{
        wiced_gpio_input_get_bool(HELP_ACCUSE,&BTN_LAMP);
     if(BTN_LAMP==WICED_TRUE){
       wiced_uart_transmit_bytes( WICED_UART_1, BTN_EVENT, sizeof(BTN_EVENT)-1 );
        isr_time_enable=WICED_TRUE;
         }

    }

//    if(is_imu_active==WICED_TRUE){
//        wiced_gpio_output_low(HELP_CONFIRM);
//
//    }
}

void timer_button(void* arg)
{
    wiced_gpio_input_get_bool(HELP_ACCUSE,&BTN_LAMP);

    if(isr_time_enable==WICED_TRUE){

        wiced_gpio_input_get_bool(HELP_ACCUSE,&BTN_LAMP);
        if(BTN_LAMP==WICED_FALSE){
            count_isr=count_isr+1;
             if(count_isr==16){
                wiced_uart_transmit_bytes( WICED_UART_1, SOS_EVENT, sizeof(SOS_EVENT)-1);
                GPIO_Aux=1;
                wiced_gpio_output_high(HELP_CONFIRM);
                isr_time_enable=WICED_FALSE;
                count_isr=0;
            }
        }
        else{count_isr=0;}

    }

}


void blink_wifi(){

    wiced_gpio_output_high(WIFI_STATUS);
    wiced_rtos_delay_milliseconds( 100 );
    wiced_gpio_output_low(WIFI_STATUS);
    wiced_rtos_delay_milliseconds( 100 );
    wiced_gpio_output_high(WIFI_STATUS);
    wiced_rtos_delay_milliseconds( 100 );
    wiced_gpio_output_low(WIFI_STATUS);
    wiced_gpio_output_high(WIFI_STATUS);
    wiced_rtos_delay_milliseconds( 100 );
    wiced_gpio_output_low(WIFI_STATUS);


}
void blink_cfg(){
//    CAMBIAR DE LED A EL DISPONIBLE

    wiced_gpio_output_high(OTA_LED);
    wiced_rtos_delay_milliseconds( 50 );
    wiced_gpio_output_low(OTA_LED);
    wiced_rtos_delay_milliseconds(10);

    wiced_gpio_output_low(OTA_LED);

}
void reset_sequence(){
    for (int v=0;v<=7;v++){
        wiced_gpio_output_high(WIFI_STATUS);
        wiced_rtos_delay_milliseconds( WIFI_BLINK);
        wiced_gpio_output_low(WIFI_STATUS);
        wiced_rtos_delay_milliseconds( WIFI_BLINK);

    }
    wiced_gpio_output_low(WIFI_STATUS);


}
void blink_stand_by(){

    /*Ectract data to peripherals*/
//
//   temp_count        = (float) adc_data.last_sample;
//   temp_volt     = (temp_count/4096)*3.3;
//
//   temp_count=temp_count*3.99975586;
//   ADC_VALUE=(int)temp_count;


    for (int v=0;v<=4;v++){
    wiced_gpio_output_high(OTA_LED);
    wiced_rtos_delay_milliseconds( 100 );
    wiced_gpio_output_low(OTA_LED);
    wiced_rtos_delay_milliseconds( 400 );
    }
    wiced_gpio_output_low(OTA_LED);
}
#endif  /* stdbool.h */
