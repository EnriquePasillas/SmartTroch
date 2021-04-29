
#ifndef _MANAGER_NET_H
#define _MANAGER_NET_H

#include "wiced.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "dct_data_write.h"
#include  "manager_menssage.h"


typedef struct net_Save
{
     char ssid [30];
     uint8_t ssid_length;
     char pass [30];
     uint8_t pass_length;
     char gate [30];
     uint8_t gate_length;
     char ipser [30];
     uint8_t ipser_length;
     char mask [30];
     uint8_t mask_length;
     char ipsta [30];
     uint8_t ipsta_length;
}net;


//Flag to start tcp service
uint8_t flag_tcp = 0;
uint8_t flag_vin=0;
uint8_t flag_charger=0;
unsigned char data_on[300];
int status_config=0;
//
////
//static wiced_result_t  is_config();
//static wiced_result_t Set_SSID(uint8_t *data,uint8_t len,wiced_uart_t uart);
//static wiced_result_t Set_KEY(uint8_t *data,uint8_t len,wiced_uart_t uart);
//static wiced_result_t Set_MASK(uint8_t *data,uint8_t len,wiced_uart_t uart);
//static wiced_result_t Set_GATEWAY(uint8_t *data,uint8_t len,wiced_uart_t uart);
//static wiced_result_t Set_SERVER(uint8_t *data,uint8_t len,wiced_uart_t uart);
//static wiced_result_t Set_IP(uint8_t *data,uint8_t len,wiced_uart_t uart);
//static wiced_result_t print_app_dct( wiced_uart_t uart );
//

static wiced_result_t Set_SSID(uint8_t *data,uint8_t len,wiced_uart_t uart){
    //dct_read_write_app_dct_t*       app_dct                  = NULL;
    wiced_result_t res;
    platform_dct_wifi_config_t*  wifi_config;
   //Se genera un cadena sin signo para almancenar temporalmente la cadena que llega , quitando los primeros dos datos
        unsigned char str_r[len];
          strcpy(str_r,&data[2]);

          // Get a copy of the WIFT config from the DCT into RAM
          wiced_dct_read_lock((void**) &wifi_config, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));

          strcpy((char *) wifi_config->stored_ap_list[0].details.SSID.value, str_r);
          wifi_config->stored_ap_list[0].details.SSID.length = strlen(str_r);

          res=wiced_dct_write((const void *) wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));
          char mensage[30];
          sprintf(mensage,"W_SSID: %s",str_r);

          if(res == WICED_SUCCESS){
              wiced_uart_transmit_bytes( uart,mensage, strlen(mensage));
            wiced_uart_transmit_bytes( uart, ("\r\n"),2);
              return WICED_SUCCESS;
            }

          wiced_dct_read_unlock(wifi_config, WICED_TRUE);


        /* Read & print all DCT sections to check that nothing has changed */
          return WICED_SUCCESS;


}
static wiced_result_t Set_KEY(uint8_t *data,uint8_t len,wiced_uart_t uart){
    //dct_read_write_app_dct_t*       app_dct                  = NULL;
        wiced_result_t res;
        platform_dct_wifi_config_t*  wifi_config;
       //Se genera un cadena sin signo para almancenar temporalmente la cadena que llega , quitando los primeros dos datos

       unsigned char str_r[len];
       strncpy(str_r,&data[2],len);
       // Get a copy of the WIFT config from the DCT into RAM
       wiced_dct_read_lock((void**) &wifi_config, WICED_TRUE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));

       strcpy((char *) wifi_config->stored_ap_list[0].security_key, str_r);
       wifi_config->stored_ap_list[0].security_key_length = strlen(str_r);

      res= wiced_dct_write((const void *) wifi_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));
       char mensage[30];
       sprintf(mensage,"W_KEY: %s",str_r);

       if(res == WICED_SUCCESS){
           wiced_uart_transmit_bytes( uart,mensage, strlen(mensage));
         wiced_uart_transmit_bytes( uart, ("\r\n"),2);
           return WICED_SUCCESS;
         }
       wiced_dct_read_unlock(wifi_config, WICED_TRUE);


     /* Read & print all DCT sections to check that nothing has changed */
       return WICED_SUCCESS;
}
static wiced_result_t Set_MASK(uint8_t *data,uint8_t len,wiced_uart_t uart){
        dct_read_write_app_dct_t*       app_dct                  = NULL;
        wiced_result_t res;

        unsigned char str_r[len];
        strcpy(str_r,&data[2]);

        /* get the App config section for modifying, any memory allocation required would be done inside wiced_dct_read_lock() */
        wiced_dct_read_lock( (void**) &app_dct, WICED_TRUE, DCT_APP_SECTION, 0, sizeof( *app_dct ) );
        /* Modify string_var by writing the whole DCT */
        strcpy( app_dct->MASK, str_r );
        res=wiced_dct_write( (const void*) app_dct, DCT_APP_SECTION, 0, sizeof(dct_read_write_app_dct_t) );
        char mensage[30];
        sprintf(mensage,"W_MASK: %s",str_r);

        if(res == WICED_SUCCESS){
            wiced_uart_transmit_bytes( uart,mensage, strlen(mensage));
          wiced_uart_transmit_bytes( uart, ("\r\n"),2);
            return WICED_SUCCESS;
          }
        wiced_dct_read_unlock( app_dct, WICED_FALSE);

        /* Read & print all DCT sections to check that nothing has changed */
        return WICED_SUCCESS;

}
static wiced_result_t print_app_dct( wiced_uart_t uart ){
        dct_read_write_app_dct_t* dct_app = NULL;
        platform_dct_wifi_config_t*  wifi_config;
        if ( wiced_dct_read_lock( (void**) &dct_app, WICED_FALSE, DCT_APP_SECTION, 0, sizeof( *dct_app ) ) != WICED_SUCCESS )
        {
            return WICED_ERROR;
        }
        // Get a copy of the WIFT config from the DCT into RAM
        wiced_dct_read_lock((void**) &wifi_config, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, 0, sizeof(platform_dct_wifi_config_t));


        /* since we passed ptr_is_writable as WICED_FALSE, we are not allowed to write in to memory pointed by dct_security */
        wiced_uart_transmit_bytes(uart,( "\nGATEWAY: " ),10);
        wiced_uart_transmit_bytes(uart,( "%s\r\n",(char*) ((dct_read_write_app_dct_t*)dct_app)->GATE ),strlen((((dct_read_write_app_dct_t*)dct_app)->GATE )));
        wiced_uart_transmit_bytes(uart,( "\nIP: " ),5);
        wiced_uart_transmit_bytes(uart,( "%s\r\n",(char*) ((dct_read_write_app_dct_t*)dct_app)->IP ),strlen((((dct_read_write_app_dct_t*)dct_app)->IP )));
        wiced_uart_transmit_bytes(uart,( "\nSERVER: " ),9);
        wiced_uart_transmit_bytes(uart,( "%s\r\n",(char*) ((dct_read_write_app_dct_t*)dct_app)->SERVER ),strlen((((dct_read_write_app_dct_t*)dct_app)->SERVER )));
        wiced_uart_transmit_bytes(uart,( "\nMASK: " ),7);
        wiced_uart_transmit_bytes(uart,( "%s\r\n",(char*) ((dct_read_write_app_dct_t*)dct_app)->MASK ),strlen((((dct_read_write_app_dct_t*)dct_app)->MASK )));
        wiced_uart_transmit_bytes(uart,( "\nSSID: " ),7);
        wiced_uart_transmit_bytes(uart,("%s\r\n",wifi_config->stored_ap_list[0].details.SSID.value),wifi_config->stored_ap_list[0].details.SSID.length);
        wiced_uart_transmit_bytes(uart,( "\nKEY: " ),6);
        wiced_uart_transmit_bytes(uart,("%s\r\n",wifi_config->stored_ap_list[0].security_key),wifi_config->stored_ap_list[0].security_key_length);
        wiced_mac_t mac;

        if ( wwd_wifi_get_mac_address( &mac, WWD_STA_INTERFACE ) == WWD_SUCCESS )
        {
          //  WPRINT_NETWORK_INFO(("WLAN MAC Address : %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]));
            char mac_r[26];
                  sprintf(mac_r,"\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",mac.octet[0],mac.octet[1],mac.octet[2],mac.octet[3],mac.octet[4],mac.octet[5]);
                  wiced_uart_transmit_bytes(uart,("%s",mac_r),strlen(mac_r));
        }
        sprintf(data_on,"\n%s|%s",(dct_read_write_app_dct_t*)dct_app->GATE,(dct_read_write_app_dct_t*)dct_app->IP);

        wiced_uart_transmit_bytes(uart,( "    \r\n" ),6);



        /* Here ptr_is_writable should be same as what we passed during wiced_dct_read_lock() */
        wiced_dct_read_unlock( dct_app, WICED_FALSE );
        wiced_dct_read_unlock(wifi_config, WICED_FALSE);

        return WICED_SUCCESS;
}
static wiced_result_t Set_IP(uint8_t *data,uint8_t len,wiced_uart_t uart){

    dct_read_write_app_dct_t*       app_dct                  = NULL;
    wiced_result_t res;
           unsigned char str_r[len];
           strcpy(str_r,&data[2]);
           /* get the App config section for modifying, any memory allocation required would be done inside wiced_dct_read_lock() */
           wiced_dct_read_lock( (void**) &app_dct, WICED_TRUE, DCT_APP_SECTION, 0, sizeof( *app_dct ) );

           /* Modify string_var by writing the whole DCT */
           strcpy( app_dct->IP, str_r );

           res=wiced_dct_write( (const void*) app_dct, DCT_APP_SECTION, 0, sizeof(dct_read_write_app_dct_t) );
           char mensage[30];
           sprintf(mensage,"W_IP: %s",str_r);
           if(res == WICED_SUCCESS){
               wiced_uart_transmit_bytes( uart,mensage, strlen(mensage));
               wiced_uart_transmit_bytes( uart, ("\r\n"),2);
               return WICED_SUCCESS;
             }

           /* release the read lock */
           wiced_dct_read_unlock( app_dct, WICED_FALSE);

         /* Read & print all DCT sections to check that nothing has changed */
           return WICED_SUCCESS;
}
static wiced_result_t Set_SERVER(uint8_t *data,uint8_t len,wiced_uart_t uart){

    dct_read_write_app_dct_t*       app_dct                  = NULL;
    wiced_result_t res;

    unsigned char str_r[len];
          strcpy(str_r,&data[2]);

           /* get the App config section for modifying, any memory allocation required would be done inside wiced_dct_read_lock() */
           wiced_dct_read_lock( (void**) &app_dct, WICED_TRUE, DCT_APP_SECTION, 0, sizeof( *app_dct ) );

           /* Modify string_var by writing the whole DCT */
           strcpy( app_dct->SERVER, str_r );

           res=wiced_dct_write( (const void*) app_dct, DCT_APP_SECTION, 0, sizeof(dct_read_write_app_dct_t) );
           char mensage[30];
           sprintf(mensage,"W_SERVER: %s",str_r);
           if(res == WICED_SUCCESS){
               wiced_uart_transmit_bytes( uart,mensage, strlen(mensage));
             wiced_uart_transmit_bytes( uart, ("\r\n"),2);
               return WICED_SUCCESS;
             }
           /* release the read lock */
           wiced_dct_read_unlock( app_dct, WICED_FALSE);

         /* Read & print all DCT sections to check that nothing has changed */
           return WICED_SUCCESS;
}
static wiced_result_t Set_GATEWAY(uint8_t *data,uint8_t len,wiced_uart_t uart){
    dct_read_write_app_dct_t*       app_dct                  = NULL;
    wiced_result_t res;
           unsigned char str_r[len];
           strcpy(str_r,&data[2]);
           /* get the App config section for modifying, any memory allocation required would be done inside wiced_dct_read_lock() */
           wiced_dct_read_lock( (void**) &app_dct, WICED_TRUE, DCT_APP_SECTION, 0, sizeof( *app_dct ) );

           /* Modify string_var by writing the whole DCT */
           strcpy( app_dct->GATE, str_r );

           res=wiced_dct_write( (const void*) app_dct, DCT_APP_SECTION, 0, sizeof(dct_read_write_app_dct_t) );
           char mensage[30];
           sprintf(mensage,"W_GATEWAY: %s",str_r);
           if(res == WICED_SUCCESS){
               wiced_uart_transmit_bytes( uart,mensage, strlen(mensage));
             wiced_uart_transmit_bytes( uart, ("\r\n"),2);
               return WICED_SUCCESS;
             }
           /* release the read lock */
           wiced_dct_read_unlock( app_dct, WICED_FALSE);

         /* Read & print all DCT sections to check that nothing has changed */
           return WICED_SUCCESS;
}

static wiced_result_t  is_config(){
    dct_read_write_app_dct_t* dct_app = NULL;

    if ( wiced_dct_read_lock( (void**) &dct_app, WICED_FALSE, DCT_APP_SECTION, 0, sizeof( *dct_app ) ) != WICED_SUCCESS )
    {
    return WICED_ERROR;
    }
    // Get a copy of the WIFT config from the DCT into RAM

    flag_tcp=(int)dct_app->F_Sate;

    wiced_dct_read_unlock( dct_app, WICED_FALSE );


return WICED_SUCCESS;
}

static wiced_result_t ready_net(struct net_Save sf){
    WPRINT_APP_INFO(("\n Setting Network\r\n\n"));

    Set_SSID(sf.ssid,sf.ssid_length,WICED_UART_1);
    Set_KEY(sf.pass,sf.ssid_length,WICED_UART_1);
    Set_SERVER(sf.ipser,sf.ipser_length,WICED_UART_1);
    Set_GATEWAY(sf.gate,sf.gate_length,WICED_UART_1);
    Set_MASK(sf.mask,sf.mask_length,WICED_UART_1);
    Set_IP(sf.ipsta,sf.ipsta_length,WICED_UART_1);



}

#endif  /* stdbool.h */
