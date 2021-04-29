#ifndef INIT_NET_H
#define INIT_NET_H

#include "wiced.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "manager_tcp_client.h"
#include "wireless_config.h"
wiced_bool_t delete_class_socket=WICED_FALSE;
wiced_bool_t netdown=WICED_FALSE;

/* WICED Join Options */
//#define WICED_JOIN_RETRY_ATTEMPTS             (500)
wiced_ip_setting_t device_init_ip_settings2;

void network_re_up(){
    wiced_bool_t upnet;
    wiced_result_t ress;
    wiced_scan_result_t *app;
//    ress=wiced_wifi_find_ap("SF-HARDWARE11",&app,7);
//    WPRINT_APP_INFO(("RESULT FIND: %d",ress));
    if(flag_tcp==3){
    upnet=wiced_network_is_up( WICED_STA_INTERFACE);

        if(wiced_network_up( WICED_STA_INTERFACE, WICED_USE_STATIC_IP, &device_init_ip_settings2 )==WICED_SUCCESS){
//            WPRINT_APP_INFO( ("evento 1\r\n") );

            }
        else{
            WPRINT_APP_INFO( ("evento 2\r\n") );

            wiced_network_down(WICED_STA_INTERFACE);
            if(delete_class_socket){
                WPRINT_APP_INFO( ("deletae socket 2\r\n") );

                wiced_tcp_delete_socket(&tcp_client_socket);

            }

        }
        if(upnet==WICED_TRUE){
//            WPRINT_APP_INFO( ("evento 1 -> 1\r\n") );
        }
        else{
//            WPRINT_APP_INFO( ("evento 1 -> 2\r\n") );
        }

    }

}

void net_lamp(){
    d1= ((rg[0]<< 24) | (rg[1] << 16) | ( rg[2] << 8) | (rg[3]));
    d2= ((rn[0]<< 24) | (rn[1] << 16) | ( rn[2] << 8) | (rn[3]));
    d3= ((ri[0]<< 24) | (ri[1] << 16) | ( ri[2] << 8) | (ri[3]));

    s1 = MAKE_IPV4_ADDRESS(rs[0],rs[1],rs[2],rs[3]);

    device_init_ip_settings2.gateway.ip.v4=(uint32_t)d1;
    device_init_ip_settings2.netmask.ip.v4=(uint32_t)d2;
    device_init_ip_settings2.ip_address.ip.v4=(uint32_t)d3;

//    wiced_rtos_register_timed_event( &re_connect, WICED_NETWORKING_WORKER_THREAD, &network_re_up, 1000, 0 );

    result = wiced_network_up(WICED_STA_INTERFACE, WICED_USE_STATIC_IP, &device_init_ip_settings2);
    WPRINT_APP_INFO( ("Net lamp\r\n") );

    /* Create a TCP socket */
    wiced_result_t res_create;
    res_create=wiced_tcp_create_socket( &tcp_client_socket, interface );
//    if(res_create){
//        WPRINT_APP_INFO( ("frist create socket \r\n") );
//
//        delete_class_socket=WICED_TRUE;
//    }

    /* Bind to the socket */
    wiced_tcp_bind( &tcp_client_socket, TCP_SERVER_PORT );
    /* Register a function to send TCP packets */
    wiced_rtos_register_timed_event( &tcp_client_event, WICED_NETWORKING_WORKER_THREAD, &tcp_client, TCP_CLIENT_INTERVAL * SECONDS, 0 );


}
void net_config(){

    /* Bring up the network interface */
    wiced_result_t result;

    Set_SSID(SIID_C,sizeof(SIID_C),WICED_UART_1);
    Set_KEY(PASS_C,sizeof(PASS_C),WICED_UART_1);

    WPRINT_APP_INFO( ("Net config\r\n") );

    result = wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER,&device_init_settings);

    /* Create a TCP socket */
    if ( wiced_tcp_create_socket( &tcp_client_socket, interface ) != WICED_SUCCESS )
    {

    }
    /* Bind to the socket */
    wiced_tcp_bind( &tcp_client_socket, TCP_SERVER_PORT_c );
    /* Register a function to send TCP packets */
    wiced_rtos_register_timed_event( &tcp_client_event, WICED_NETWORKING_WORKER_THREAD, &tcp_client_config, TCP_CLIENT_INTERVAL_c, 0 );
}


#endif  /* stdbool.h */
