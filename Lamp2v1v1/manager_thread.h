 #ifndef _MANAGER_THREAD_H
#define _MANAGER_THREAD_H
#include "stdbool.h"


#include "wiced.h"
#include "stdbool.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"




unsigned char set_net[6];
char* res_set;
#define BUFFER_RX_LEN       120
#define BUFFER_RX_WIDTH     120
#define BUFFER_SIZE    120

/* Thread parameters */
#define THREAD_PRIORITY     (10)
#define THREAD_STACK_SIZE   (1024)

/* The queue messages will be 4 bytes each (a 32 bit integer) */
#define MESSAGE_SIZE        (64) // 10 bites for every massage, if need chain's more complex increment this value
#define QUEUE_SIZE          (1)

static wiced_queue_t queueHandle1;
static wiced_queue_t queueHandle3;

static wiced_thread_t uart1_ThreadHandle;
static wiced_thread_t uart3_ThreadHandle;
static wiced_thread_t uart_main1_ThreadHandle;
static wiced_thread_t uart_main3_ThreadHandle;

uint32_t expected_data_size = 1;
uint8_t thread1, thread2, thread3;

wiced_uart_config_t uart_config = {
        .baud_rate      = 115200,
        .data_width     = DATA_WIDTH_8BIT,
        .parity         = NO_PARITY,
        .stop_bits      = STOP_BITS_1,
        .flow_control   = FLOW_CONTROL_DISABLED,
};


//------------------------------------------------------------- UART QUEUE 1 --------------------------------------------------------
void uart1_thread(wiced_thread_arg_t arg){
    char rx_buffer1[10];
    char recive1;
    uint8_t i = 0;
    wiced_uart_init(WICED_UART_1, &uart_config, NULL);
    wiced_uart_transmit_bytes(WICED_UART_1, VERSION, sizeof(VERSION)-1);
    wiced_uart_transmit_bytes(WICED_UART_1, UART1, sizeof(UART1)-1);
    while(1){
        if ( wiced_uart_receive_bytes( WICED_UART_1, &recive1, &expected_data_size, WICED_NEVER_TIMEOUT ) == WICED_SUCCESS ){
            if(recive1 == '\n'){
                if(i==0){
                    memset(rx_buffer1,'\0',10);
                    memset(&recive1,'\0',i);
                    thread1 = i;
                    i=0;
                    expected_data_size=1;

                }
                else{
                    wiced_rtos_push_to_queue(&queueHandle1, &rx_buffer1, WICED_NO_WAIT); /* Push value onto queue*/
                    memset(rx_buffer1,'\0',10);
                    memset(&recive1,'\0',i);
                    thread1 = i;
                    i=0;
                    expected_data_size=1;
                }
            }
            //else if(recive1 == '\r'){}
            else{ rx_buffer1[i++] = recive1; }
        }

    }
}

//------------------------------------------------------------- UART QUEUE 3 --------------------------------------------------------
void uart3_thread(wiced_thread_arg_t arg){
    char rx_buffer3[10];
    char recive3;
    uint8_t k = 0;
    wiced_uart_init(WICED_UART_3, &uart_config, NULL);
    wiced_uart_transmit_bytes(WICED_UART_3, VERSION, sizeof(VERSION)-1);
    wiced_uart_transmit_bytes(WICED_UART_3, UART3, sizeof(UART3)-1);
    wiced_uart_transmit_bytes(WICED_UART_3, GET_MAC_BT, sizeof(GET_MAC_BT)-1);
    while(1){
        if ( wiced_uart_receive_bytes( WICED_UART_3, &recive3, &expected_data_size, WICED_NEVER_TIMEOUT ) == WICED_SUCCESS ){
            if(recive3 == '\n'){
                if(k==0){
                    memset(rx_buffer3,'\0',10);
                    memset(&recive3,'\0',k);
                    thread1 = k;
                    k=0;
                    expected_data_size=1;

                }
                else{
                    wiced_rtos_push_to_queue(&queueHandle3, &rx_buffer3, WICED_NO_WAIT); /* Push value onto queue*/
                    memset(rx_buffer3,'\0',10);
                    memset(&recive3,'\0',k);
                    thread3 = k;
                    k=0;
                    expected_data_size=1;
                }

            }
            //else if(recive3 == '\r'){}
            else{ rx_buffer3[k++] = recive3; }
        }
    }

}

//------------------------------------------------------------- MAIN UART 1 ---------------------------------------------------
void uart_main1(wiced_thread_arg_t arg){
    char message1[100];
    wiced_mac_t macx;
    char mac_tx[90];

    char* res;              //Point char required for strstr (compare first argument vs some character that include same value)
    while(1){
        wiced_rtos_pop_from_queue(&queueHandle1, &message1, WICED_WAIT_FOREVER);
        wiced_uart_transmit_bytes(WICED_UART_1, &message1, thread1);
        wiced_uart_transmit_bytes(WICED_UART_1, "\r\n", 2);
        res = strstr(&message1,"OTA WIFI");  //Requiere puntero en lugar de caracter directo para comparar
        if(res){
               wiced_uart_transmit_bytes( WICED_UART_3, OTA, sizeof( OTA ) - 1 );
           }
        res = strstr(&message1,"ZOM_BT");
        if(res){
          wiced_uart_transmit_bytes( WICED_UART_3, ZOM_BT, sizeof( ZOM_BT ) - 1 );
        }

        res = strstr(&message1,"BSERVER");
        if(res){
            Down_net();
            wiced_framework_reboot();

        }

        res = strstr(&message1,"CONN");
          if(res){
              identifique();
          }

          res = strstr(&message1,"NSH");
          if(res){
              flag_hc_is_active=WICED_TRUE;
              GPIO_activate=1;
          }

        thread1 = 0;
        wiced_rtos_delay_milliseconds(100);
    }
}

//------------------------------------------------------------- MAIN UART 3 --------------------------------------------------------
void uart_main3(wiced_thread_arg_t arg){
    char message3[100];
    char* res;
    while(1){
        wiced_rtos_pop_from_queue(&queueHandle3, &message3, WICED_WAIT_FOREVER);
        res = strstr(&message3,"BNM:");  //Requiere puntero en lugar de caracter directo para comparar
        if(res){
        }
        res = strstr(&message3,"BLE");
        if(res){
           count_xrest_bt=0;

        }
        res = strstr(&message3,"macx:");
        if(res){
//            strcpy(MAC_BT_FACTORY,&message3[4]);
//            WPRINT_APP_INFO(("ID MAC BT= %s\n",MAC_BT_FACTORY));
        }

        res=strstr(&message3,"CENTRAL");
        if(res){
            BLE_flag = WICED_TRUE;
        }
        res = strstr(&message3,"BSE");
        if(res){
            Down_net();
            wiced_framework_reboot();

        }
        thread3 = 0;
        wiced_rtos_delay_milliseconds(100);
    }
}
#endif  /* stdbool.h */
