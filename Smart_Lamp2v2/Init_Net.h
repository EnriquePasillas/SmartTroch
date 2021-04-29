#ifndef INIT_NET_H
#define INIT_NET_H

#include "wiced.h"
#include "string.h"
#include "stdbool.h"
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

//#include  "manager_bt.h"
#include  "i2c_lsm6dsm.h"
#include "wireless_config.h"
/* WICED Join Options */
//#define WICED_JOIN_RETRY_ATTEMPTS             (80)

void net_lamp(){
    wiced_result_t result;

      d1= ((rg[0]<< 24) | (rg[1] << 16) | ( rg[2] << 8) | (rg[3]));
      d2= ((rn[0]<< 24) | (rn[1] << 16) | ( rn[2] << 8) | (rn[3]));
      d3= ((ri[0]<< 24) | (ri[1] << 16) | ( ri[2] << 8) | (ri[3]));

      s1 = MAKE_IPV4_ADDRESS(rs[0],rs[1],rs[2],rs[3]);

      wiced_ip_setting_t device_init_ip_settings2 ={
      .gateway={WICED_IPV4,{.v4=(uint32_t)d1}},
      .netmask={WICED_IPV4,{.v4=(uint32_t)d2}},
      .ip_address={WICED_IPV4,{.v4=(uint32_t)d3}},

      };
      DIIS=device_init_ip_settings2;
      result = wiced_network_up(WICED_STA_INTERFACE, WICED_USE_STATIC_IP, &device_init_ip_settings2);


/* Create a TCP socket */

     if ( wiced_tcp_create_socket( &tcp_client_socket, interface ) != WICED_SUCCESS )
     {

     }
     else{

     }
     /* Bind to the socket */
     wiced_tcp_bind( &tcp_client_socket, TCP_SERVER_PORT );
    /* Register a function to send TCP packets */
    wiced_rtos_register_timed_event( &tcp_client_event, WICED_NETWORKING_WORKER_THREAD, &tcp_client, TCP_CLIENT_INTERVAL * SECONDS, 0 );


}
void net_config(){
    /* Bring up the network interface */
    wiced_result_t result;

    result = wiced_network_up(WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, &device_init_settings);
    if ( result != WICED_SUCCESS )
    {

    }

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
