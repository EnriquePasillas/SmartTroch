
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                     Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
#pragma pack (1)
typedef struct
{
    char        MASK[20];
    char        GATE[20];
    char        IP[20];
    char        SERVER[20];
    char        MAC_BT[20];
    uint8_t         F_config;
    uint8_t         F_save;
    uint8_t         enable_rx;
    uint8_t         gpio_aux;
    uint8_t         gpio_recib;
    uint8_t         gpio_evac;
    uint8_t         gpio_fall;
    uint8_t         gpio_inacty;
    uint8_t         acc_xy;
    uint8_t         acc_z_min;
    uint8_t         acc_z_plus;
    uint8_t         blink_tcp;
    uint8_t         blink_awake;
    uint8_t         F_charger;
    uint8_t         F_chr;
    uint8_t         F_enable_btn;
    uint8_t         F_Sate;

} dct_read_write_app_dct_t;

#pragma pack ()
/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /*extern "C" */
#endif
