#ifndef _WIRELESS_CONFIG_H
#define _WIRELESS_CONFIG_H

#include "wiced.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "dct_data_write.h"

#include "manager_net.h"
#include "manager_thread.h"


wiced_tcp_stream_t* tcp_stream_wr;
wiced_packet_t*          packet;
wiced_packet_t*          rx_packet;

//char sendMessage[80];
unsigned char* sendMessage;
unsigned char*                    tx_data;

#define TCP_CLIENT_INTERVAL_c               100
#define TCP_SERVER_PORT_c                   8077

/* Change the server IP address to match the TCP echo server address */
#define TCP_SERVER_IP_ADDRESS MAKE_IPV4_ADDRESS(10,86,10,1)




struct net_Save net_value;


static wiced_result_t tcp_client();

/******************************************************
 *               Variable Definitions
 ******************************************************/

static const wiced_ip_setting_t device_init_settings =
{
    INITIALISER_IPV4_ADDRESS( .ip_address, MAKE_IPV4_ADDRESS(10,86,  11,  1) ),
    INITIALISER_IPV4_ADDRESS( .netmask,    MAKE_IPV4_ADDRESS(255,255,255,  0) ),
    INITIALISER_IPV4_ADDRESS( .gateway,    MAKE_IPV4_ADDRESS(10,86,  10,  254) ),
};

static wiced_tcp_socket_t  tcp_client_socket;
void select_net(uint8_t* msn1,uint8_t thread1);


wiced_result_t tcp_client_config( void* arg )
{
    wiced_result_t result;
    wiced_mac_t myMac;
    wiced_ip_address_t myIpAddress;
    wl_bss_info_t ap_info_buffer;
    wiced_security_t ap_security;
    unsigned char*                    rx_data;
    uint16_t                 rx_data_length;
    uint16_t                 available_data_length;
    int                      connection_retries;
    wiced_ip_address_t INITIALISER_IPV4_ADDRESS( server_ip_address, TCP_SERVER_IP_ADDRESS );

         /* Connect to the remote TCP server, try several times */
         connection_retries = 0;
         do
         {
             result = wiced_tcp_connect( &tcp_client_socket, &server_ip_address, TCP_SERVER_PORT_c, TCP_CLIENT_CONNECT_TIMEOUT );
             connection_retries=connection_retries+1;
         }
         while( ( result != WICED_SUCCESS ) && ( connection_retries < TCP_CONNECTION_NUMBER_OF_RETRIES ) );
         if ( result != WICED_SUCCESS )
            {
            WPRINT_APP_INFO(("Unable to connect to the server! Halt.\n"));
            try_n=try_n+1;
            if(try_n==240){
                wiced_framework_reboot();

            }
            wiced_uart_transmit_bytes(WICED_UART_1,TCP_CONNECT_FAILED,sizeof(TCP_CONNECT_FAILED)-1);
            return WICED_ERROR;

         }
         /* Create the TCP packet. Memory for the tx_data is automatically allocated */
         if (wiced_packet_create_tcp(&tcp_client_socket, TCP_PACKET_MAX_DATA_LENGTH, &packet, (uint8_t**)&sendMessage, &available_data_length) != WICED_SUCCESS)
         {
             WPRINT_APP_INFO(("TCP packet creation failed\n"));
             try_n=try_n+1;
             if(try_n==240){
                 wiced_framework_reboot();

             }
            wiced_uart_transmit_bytes(WICED_UART_1,CREATE_FAILED,sizeof(CREATE_FAILED)-1);

             return WICED_ERROR;
         }

         /* Write the message into sendMessage"  */
       switch_msm(status_config);

         /* Set the end of the data portion */
        wiced_packet_set_data_end(packet, (uint8_t*)sendMessage + strlen(sendMessage));
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
             wiced_uart_transmit_bytes(WICED_UART_1,(("%s\r\n",sendMessage)),strlen(sendMessage)); // echo the message so that the user can see something

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
                     /* Delete packet, since the receive failed */
                    wiced_packet_delete(rx_packet);

                     /* Close the connection */
                    wiced_tcp_disconnect(&tcp_client_socket);
                     return WICED_ERROR;
                 }

                 /* Null terminate the received string */
                rx_data[rx_data_length] = '\x0';
//                WPRINT_APP_INFO(("%s",rx_data));

                WPRINT_APP_INFO(("\nDato: %s\n",rx_data));
                   select_net(rx_data,strlen(rx_data)-1);

         wiced_packet_delete(packet);
         wiced_packet_delete(rx_packet);
         wiced_tcp_disconnect(&tcp_client_socket);

        return WICED_SUCCESS;


}

void select_net(uint8_t* message1,uint8_t thread1 ){
    char* res;
       res = strstr(message1,"Starting_Config");  //Bandera para dar incio a la configuracion de red
       if(res){
            status_config=1;
       }
       res = strstr(message1,"-S");  //Bandera para setear en memoria la vlan o ssid
       if(res){
           status_config=2;
//           strncpy(net_value.ssid,message1,thread1);
           strcpy(net_value.ssid,message1);
           net_value.ssid_length=thread1;
//           Set_SSID(message1,thread1,WICED_UART_1);
       }
       res = strstr(message1,"-A");
       if(res){
         print_app_dct(WICED_UART_1);
       }
       res = strstr(message1,"-K");  //Bnadera para setear en memoroa la contraseña de la vlan o ssid
       if(res){
          status_config=3;
//           Set_KEY(message1,thread1,WICED_UART_1);
           strcpy(net_value.pass,message1);
           net_value.pass_length=thread1;
       }
       res = strstr(message1,"-I");  //Bandera para setear en memoria la ip del servidor
       if(res){
            status_config=5;
//            Set_SERVER(message1,thread1,WICED_UART_1);
            strcpy(net_value.ipser,message1);
            net_value.ipser_length=thread1;
       }
       res = strstr(message1,"-G");  //Bandera para setear en memoria el gateway del servidor
       if(res){
            status_config=4;
//            Set_GATEWAY(message1,thread1,WICED_UART_1);
            strcpy(net_value.gate,message1);
            net_value.gate_length=thread1;
       }
       res = strstr(message1,"-M");  //Bandera para setear en memoria la mac del servidor
       if(res){
            status_config=6;
//            Set_MASK(message1,thread1,WICED_UART_1);
            strcpy(net_value.mask,message1);
            net_value.mask_length=thread1;
       }
       res = strstr(message1,"-C");  //Bandera para setear en memoria la ip del client
       if(res){
            status_config=7;
//            Set_IP(message1,thread1,WICED_UART_1);
            strcpy(net_value.ipsta,message1);
            net_value.ipsta_length=thread1;
       }
        res = strstr(message1,"-E");  //Bandera para dar reinicio al modulo y tome los nuevos valores de la memoria
       if(res){
                Set_config();
                ready_net(net_value);
            wiced_packet_delete(packet);
            wiced_packet_delete(rx_packet);
            wiced_tcp_disconnect(&tcp_client_socket);
            wiced_framework_reboot();

        }
       res = strstr(message1,"FLASH");  //Requiere puntero en lugar de caracter directo para comparar
       if(res){
            identifique();
       }
       res = strstr(message1,"All_Over_Again");  //Requiere puntero en lugar de caracter directo para comparar
       if(res){
           status_config=0;
       }
       res = strstr(message1,"B>WBN");  //Requiere puntero en lugar de caracter directo para comparar
        if(res){
//            repeat_bt(message1,thread1,WICED_UART_1);
        }
        res = strstr(message1,"B>WMA");  //Requiere puntero en lugar de caracter directo para comparar
        if(res){
//            repeat_bt(message1,thread1,WICED_UART_1);
        }
        res = strstr(message1,"B>CMA");  //Requiere puntero en lugar de caracter directo para comparar
        if(res){
//            repeat_bt(message1,thread1,WICED_UART_1);
        }
        res = strstr(message1,"MACBT_R");  //Requiere puntero en lugar de caracter directo para comparar
        if(res){
            status_config=8 ;
         }

}


void switch_msm(int status_c){
    wiced_mac_t mac;
    switch(status_c  ){
    case 0:
        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
         {
            sprintf(sendMessage,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]);
         }
        break;
    case 1:
        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
         {
            sprintf(sendMessage,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n%s\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5],OK_A);
                }
            break;
    case 2:
        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
        {
            sprintf(sendMessage,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n%s\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5],OK_SC);
        }
            break;
    case 3:
        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
        {
             sprintf(sendMessage,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n%s\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5],OK_K);
        }
            break;
    case 4:
        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
        {
                sprintf(sendMessage,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n%s\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5],OK_G);
        }
            break;
    case 5:
        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
        {
            sprintf(sendMessage,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n%s\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5],OK_I);
        }
            break;
    case 6:
        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
        {
            sprintf(sendMessage,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n%s\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5],OK_M);
        }
            break;
    case 7:
        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
        {
            sprintf(sendMessage,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n%s\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5],OK_C);
        }
            break;
    case 8:
        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
        {
            sprintf(sendMessage,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n MACBT:\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]);
        }
            break;
    default:
        break;

    }

}


#endif  /* stdbool.h */
