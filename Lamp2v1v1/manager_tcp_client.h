#ifndef _MANAGER_TCP_CLIENT_H
#define _MANAGER_TCP_CLIENT_H
#include "stdbool.h"
#include "wiced.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "dct_data_write.h"
#include "manager_dct.h"

int counter_tcp_guardian;

/******************************************************
 *                     TCP Client
 ******************************************************/
//This is needed to implement tcp services
#define TCP_PACKET_MAX_DATA_LENGTH        160
#define TCP_CLIENT_INTERVAL               5


#define TCP_SERVER_PORT                   8010
#define TCP_CLIENT_CONNECT_TIMEOUT        1000
#define TCP_CLIENT_RECEIVE_TIMEOUT        1000
#define TCP_CONNECTION_NUMBER_OF_RETRIES  6

//Flag to start tcp service
wiced_interface_t interface;

//Wifi status
wiced_bool_t WIFI_ACTIVITY=WICED_FALSE;
wiced_bool_t wifi_status_led=WICED_TRUE;

uint8_t blink_imu=0;
static wiced_timer_t timerHandle_activity;

uint8_t try_n=0;
// Variables for network change
uint32_t rs[4];
uint32_t rn[4];
uint32_t ri[4];
uint32_t rg[4];

uint32_t d1;
uint32_t d2;
uint32_t d3;
uint32_t d4;
unsigned long s1;
/******************************************************
 *                    Variable
 ******************************************************/


//wiced_bool_t Ctr_Connection;

wiced_bool_t  Acuse_Ayuda=0;

wiced_result_t result;

wiced_ip_setting_t DIIS;


int ADC_VALUE;

wiced_bool_t yes=0;


static wiced_result_t init_tcp_w();
static wiced_result_t tcp_client( void );
static wiced_tcp_socket_t  tcp_client_socket;
static wiced_timed_event_t tcp_client_event;

static wiced_result_t guardian_v( void );

static wiced_timed_event_t guardian;


static wiced_result_t init_tcp_w(){
    dct_read_write_app_dct_t* dct_app = NULL;
    wiced_result_t result;
    char *p1;
    char *p2;
    char *p3;
    int x=0;


    if ( wiced_dct_read_lock( (void**) &dct_app, WICED_FALSE, DCT_APP_SECTION, 0, sizeof( *dct_app ) ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    char cad_aux[sizeof(dct_app->IP)];
    /* Modify string_var by writing the whole DCT */
    strcpy(cad_aux,(char*) (dct_read_write_app_dct_t*)dct_app->IP);

    //establece como  realizara el split
    char delim[] = ".";
    char *ptr1 = strtok(cad_aux, delim);

    while(ptr1 != NULL)
    {

        ri[x]= strtol(ptr1, &p1, 10);
        x++;
        ptr1=strtok(NULL, delim);
    }
    x=0;

    /* Modify string_var by writing the whole DCT */
    strcpy(cad_aux,(char*) (dct_read_write_app_dct_t*)dct_app->MASK);

    //establece como  realizara el split
    char *ptr2 = strtok(cad_aux, delim);

    while(ptr2 != NULL)
    {

        rn[x]= strtol(ptr2, &p2, 10);
        x++;
        ptr2=strtok(NULL, delim);
    }
    x=0;

    /* Modify string_var by writing the whole DCT */
    strcpy(cad_aux,(char*) (dct_read_write_app_dct_t*)dct_app->SERVER);

    //establece como  realizara el split
    char *ptr3 = strtok(cad_aux, delim);

    while(ptr3 != NULL)
    {

        rs[x]= strtol(ptr3, &p3, 10);
        x++;
        ptr3=strtok(NULL, delim);
    }
    x=0;


    d1= ((rg[0]<< 24) | (rg[1] << 16) | ( rg[2] << 8) | (rg[3]));
    d2= ((rn[0]<< 24) | (rn[1] << 16) | ( rn[2] << 8) | (rn[3]));
    d3= ((ri[0]<< 24) | (ri[1] << 16) | ( ri[2] << 8) | (ri[3]));
    d4= ((rs[0]<< 24) | (rs[1] << 16) | ( rs[2] << 8) | (rs[3]));

    wiced_dct_read_unlock( dct_app, WICED_FALSE);

    /* Read & print all DCT sections to check that nothing has changed */
    return WICED_SUCCESS;

}

static wiced_result_t tcp_client( void )
{
    WPRINT_APP_INFO(("Event Thread Tcp client\n"));
    counter_tcp_guardian=0;

    wiced_result_t result;
    wiced_mac_t myMac;
    wiced_ip_address_t myIpAddress;
    wl_bss_info_t ap_info_buffer;
    wiced_security_t ap_security;

    wiced_packet_t*          packet;
    wiced_packet_t*          rx_packet;
    unsigned char*                    tx_data;
    unsigned char*                    rx_data;
    uint16_t                 rx_data_length;
    uint16_t                 available_data_length;
    int                      connection_retries;
         wiced_ip_address_t INITIALISER_IPV4_ADDRESS( server_ip_address, s1 );

         /* Connect to the remote TCP server, try several times */
         connection_retries = 0;
         do
         {
             result = wiced_tcp_connect( &tcp_client_socket, &server_ip_address, TCP_SERVER_PORT, TCP_CLIENT_CONNECT_TIMEOUT );
             connection_retries=connection_retries+1;
         }
         while( ( result != WICED_SUCCESS ) && ( connection_retries < TCP_CONNECTION_NUMBER_OF_RETRIES ) );
         if ( result != WICED_SUCCESS )
            {
            WPRINT_APP_INFO(("Unable to connect to the server! Halt.\n"));
            try_n=try_n+1;
            if(try_n==3){
                wiced_framework_reboot();
            }
            wiced_uart_transmit_bytes(WICED_UART_1,TCP_CONNECT_FAILED,sizeof(TCP_CONNECT_FAILED)-1);
            return WICED_ERROR;

         }
         /* Create the TCP packet. Memory for the tx_data is automatically allocated */
         if (wiced_packet_create_tcp(&tcp_client_socket, TCP_PACKET_MAX_DATA_LENGTH, &packet, (uint8_t**)&tx_data, &available_data_length) != WICED_SUCCESS)
         {
             WPRINT_APP_INFO(("TCP packet creation failed\n"));
             try_n=try_n+1;
             if(try_n==3){
                 wiced_framework_reboot();
             }
            wiced_uart_transmit_bytes(WICED_UART_1,CREATE_FAILED,sizeof(CREATE_FAILED)-1);

             return WICED_ERROR;
         }

         /* Write the message into sendMessage"  */

        // Format the data per the specification in section 6
        wiced_wifi_get_mac_address(&myMac);                             // Se obtiene la MAC del dispositivo
        wiced_ip_get_ipv4_address( WICED_STA_INTERFACE, &myIpAddress);  // Se obtiene la IP del dispositivo
        wwd_wifi_get_ap_info(&ap_info_buffer, &ap_security);            // Se obtiene la MAC de la red a la que estamos conectados

        /*Ectract data to peripherals*/

        temp_count        = (float) adc_data.last_sample;
        temp_volt     = (temp_count/4096)*3.3;

        temp_count=temp_count*3.99975586;
        ADC_VALUE=(int)temp_count;

        memset(mcu_gpio,'0',17);


        mcu_gpio[11]=GPIO_falledman+48;
        mcu_gpio[12]=GPIO_activate+48;
        mcu_gpio[13]=GPIO_Evac+48;
        mcu_gpio[15]=GPIO_Recib+48;
        mcu_gpio[14]=GPIO_Aux+48;
        sprintf(mcu,"%s",mcu_gpio);


        if(faslh_imu==WICED_TRUE){
            identifique();
            faslh_imu=WICED_FALSE;
        }

        set_gpio_menssage(AYUDA,GPIO_Aux);
        set_gpio_menssage(CONFIRMACION,GPIO_Recib);
        set_gpio_menssage(EVACUACION,GPIO_Evac);
        set_gpio_menssage(ENABLE_A1,EnableA1);


        sprintf(tx_data,"\nL;%02X:%02X:%02X:%02X:%02X:%02X,%d,%17s,%02X:%02X:%02X:%02X:%02X:%02X,%u.%u.%u.%u\r\n",
                myMac.octet[0],myMac.octet[1],myMac.octet[2],myMac.octet[3],myMac.octet[4],myMac.octet[5],ADC_VALUE,mcu,ap_info_buffer.BSSID.octet[0], ap_info_buffer.BSSID.octet[1],
                ap_info_buffer.BSSID.octet[2],ap_info_buffer.BSSID.octet[3],ap_info_buffer.BSSID.octet[4],ap_info_buffer.BSSID.octet[5],
                (uint8_t)(GET_IPV4_ADDRESS(myIpAddress) >> 24),
                (uint8_t)(GET_IPV4_ADDRESS(myIpAddress) >> 16),
                (uint8_t)(GET_IPV4_ADDRESS(myIpAddress) >> 8),
                (uint8_t)(GET_IPV4_ADDRESS(myIpAddress) >> 0)
                );

        /* Set the end of the data portion */
         wiced_packet_set_data_end(packet, (uint8_t*)tx_data + strlen(tx_data));
         /* Send the TCP packet */
                  if (wiced_tcp_send_packet(&tcp_client_socket, packet) != WICED_SUCCESS)
                  {
                      /* Delete packet, since the send failed */
                      wiced_packet_delete(packet);

                      /* Close the connection */
                      wiced_tcp_disconnect(&tcp_client_socket);
                      return WICED_ERROR;
                  }
                  else{
                     wiced_uart_transmit_bytes(WICED_UART_1,(("%s\r\n",tx_data)),strlen(tx_data)); // echo the message so that the user can see something
                     blink_wifi();
                  }

         result = wiced_tcp_receive(&tcp_client_socket, &rx_packet, TCP_CLIENT_RECEIVE_TIMEOUT);
         if( result != WICED_SUCCESS )
                 {

                     /* Delete packet, since the receive failed */
                     wiced_packet_delete(rx_packet);

                     /* Close the connection */
                    wiced_tcp_disconnect(&tcp_client_socket);
                     return WICED_ERROR;
                 }
                 else{
                 }
                 /* Get the contents of the received packet */
                 wiced_packet_get_data(rx_packet, 0, (uint8_t**)&rx_data, &rx_data_length, &available_data_length);

                 if (rx_data_length != available_data_length)
                 {
//                     WPRINT_APP_INFO(("Fragmented packets not supported\n"));

                     /* Delete packet, since the receive failed */
                    wiced_packet_delete(rx_packet);

                     /* Close the connection */
                    wiced_tcp_disconnect(&tcp_client_socket);
                     return WICED_ERROR;
                 }

                 /* Null terminate the received string */
                rx_data[rx_data_length] = '\x0';
                WPRINT_APP_INFO(("%s",rx_data));
                Take_Action(rx_data);
//                char* resultado;
//                resultado = strstr(rx_data,"BSERVER");   // Busca si existe la palabra P1
                   if(strstr(rx_data,"BSERVER")){
                           Down_net();
                           wiced_packet_delete(packet);
                           wiced_packet_delete(rx_packet);
                           wiced_tcp_disconnect(&tcp_client_socket);
                           wiced_framework_reboot();
                   }
         wiced_packet_delete(packet);
         wiced_packet_delete(rx_packet);
         wiced_tcp_disconnect(&tcp_client_socket);

        return WICED_SUCCESS;
}




void Take_Action(uint8_t* action){
    char* resultado;
    if (EnableA1 == 0)
    {
        //char* resultado;
        resultado = strstr(action,"A1");   // Busca si existe la palabra A1
        if (resultado)
            {
                WPRINT_APP_INFO(("ID A1 \n"));
                GPIO_Evac = 1;                  // Cambio de estado de GPIO
                set_gpio_menssage(EVACUACION,GPIO_Evac);
                mcu_gpio[13]=GPIO_Evac;
                EnableA1 = 1;
                set_gpio_menssage(ENABLE_A1,EnableA1);
                identifique();
                wiced_gpio_output_high(HELP_CONFIRM);
            }
    }

    /* DA proceso */
    if (EnableA1 == 1)
    {
        //char* resultado;
        resultado = strstr(action,"DA");   // Busca si existe la palabra A1
        if (resultado)
            {
                WPRINT_APP_INFO(("ID DA \n"));
                GPIO_Evac = 0;                  // Cambio de estado de GPIO
                GPIO_Recib = 0;
                set_gpio_menssage(EVACUACION,GPIO_Evac);
                set_gpio_menssage(CONFIRMACION,GPIO_Recib);
                mcu_gpio[13]=GPIO_Evac;
                mcu_gpio[15]=GPIO_Recib;
                EnableA1 = 0;
                set_gpio_menssage(ENABLE_A1,EnableA1);
                wiced_gpio_output_low(HELP_CONFIRM);
            }
    }
    resultado = strstr(action,"N1");   // Busca si existe la palabra N1
    if(resultado)
    {
        WPRINT_APP_INFO(("ID N1 \n"));
        GPIO_Aux = 0;                  // Cambio de estado de GPIO
        set_gpio_menssage(AYUDA,GPIO_Aux);
        mcu_gpio[14]=GPIO_Aux;
        wiced_gpio_output_low(HELP_CONFIRM);
    }
    resultado = strstr(action,"P1");   // Busca si existe la palabra P1
    if(resultado)
    {
       WPRINT_APP_INFO(("ID P1 \n"));
    }
    resultado = strstr(action,"ACK");   // Busca si existe la palabra P1
    if(resultado)
    {
      WPRINT_APP_INFO(("ID P1 \n"));

    }
    resultado = strstr(action,"NNH");   // Busca si existe la palabra P1
    if(resultado)
    {
        GPIO_activate=0;
        set_gpio_menssage(INACTIVITY,GPIO_activate);
        GPIO_falledman=0;
        set_gpio_menssage(FALLENMEN,GPIO_falledman);
        flag_hc_is_active=WICED_FALSE;

        }
resultado = strstr(action,"ota_wifi");   // Busca si existe la palabra P1
   if(resultado)
   {
       wiced_uart_transmit_bytes(WICED_UART_3, "OTA\r\n", 3);
   }
resultado = strstr(action,"NFH");   // Busca si existe la palabra P1
  if(resultado)
  {
      GPIO_falledman=0;
      set_gpio_menssage(FALLENMEN,GPIO_falledman);
      wiced_rtos_start_timer(&hc_timer);
      hc_counter=0;
  }
  resultado = strstr(action,"NSH");   // Busca si existe la palabra P1
     if(resultado)
     {
         flag_hc_is_active=WICED_TRUE;
         GPIO_activate=1;
         set_gpio_menssage(INACTIVITY,GPIO_activate);
     }
}


#endif  /* stdbool.h */
