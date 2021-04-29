#ifndef STARTER_H
#define STARTER_H

#include "stdbool.h"
#include "wiced.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "i2c_lsm6dsm.h"
#include "manager_gpio.h"
#include "manager_net.h"
#include "manager_thread.h"
#include "manager_adc.h"
#include "manager_timer.h"
#include "manager_tcp_client.h"
#include "wireless_config.h"


void init_all_queue();
void init_thread_Adc();
void init_all_gpio_memory();

void system_starter(){
    imu_init();
    init_tcp_w();  /*check* the nets values on memory*/
    set_gpio();
    flag_tcp=is_config();    /*  check if the values networking is set */
    init_all_queue();
    init_thread_Adc();
    init_all_gpio_memory();
    init_all_timer();

/*Interrupcion del boton de la lampara*/
    wiced_gpio_input_irq_enable(HELP_ACCUSE, IRQ_TRIGGER_BOTH_EDGES, btn_lamp_isr, NULL); /* Setup interrupt */

/*Interrupcion para indicar la entrada del cargador*/
    wiced_gpio_input_irq_enable(VOLTAGE_LEVEL, IRQ_TRIGGER_BOTH_EDGES, divisor_isr, NULL); /* Setup interrupt */

/*  Start mcu values  */
    memset(mcu_gpio,'0',17);

}

void init_all_queue(){
/* Initialize the queue */
    wiced_rtos_init_queue(&queueHandle1, "uart_1", MESSAGE_SIZE, QUEUE_SIZE); /* Setup the semaphore which will be set by the button interrupt */
    wiced_rtos_init_queue(&queueHandle3, "uart_3", MESSAGE_SIZE, QUEUE_SIZE); /* Setup the semaphore which will be set by the button interrupt */

    /* Initialize and start UART_1 thread */
    wiced_rtos_create_thread(&uart1_ThreadHandle, THREAD_PRIORITY, "uart_1", uart1_thread, THREAD_STACK_SIZE, NULL);
    wiced_rtos_create_thread(&uart3_ThreadHandle, THREAD_PRIORITY, "uart_3", uart3_thread, THREAD_STACK_SIZE, NULL);

    wiced_rtos_create_thread(&uart_main1_ThreadHandle, THREAD_PRIORITY, "main_uart1", uart_main1, THREAD_STACK_SIZE, NULL);
    wiced_rtos_create_thread(&uart_main3_ThreadHandle, THREAD_PRIORITY, "main_uart3", uart_main3, THREAD_STACK_SIZE, NULL);


}


void init_thread_Adc(){
   /*  Initialize adc*/
        /* Initialise adc data. adc data are shared among multiple threads; therefore a mutex is required */
        memset( &adc_data, 0, sizeof( adc_data ) );
        wiced_rtos_init_mutex( &adc_data.mutex );

    /* Setup a timed event that will take a measurement */
        wiced_rtos_register_timed_event( &adc_timed_event, WICED_HARDWARE_IO_WORKER_THREAD, take_adc_reading, 1 * SECONDS, 0 );

    /*adc start*/
        wiced_adc_start();
}


void init_all_gpio_memory(){
/*  Get value gpios*/
    GPIO_Recib=get_gpio_menssage(CONFIRMACION);
    GPIO_Aux=get_gpio_menssage(AYUDA);
    GPIO_Evac=get_gpio_menssage(EVACUACION);
    EnableA1=get_gpio_menssage(ENABLE_A1);
    flag_hc_is_active=get_gpio_menssage(INACTIVITY);
    GPIO_activate=get_gpio_menssage(INACTIVITY);
    GPIO_falledman=get_gpio_menssage(FALLENMEN);
}





#endif  /* stdbool.h */
