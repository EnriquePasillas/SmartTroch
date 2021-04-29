#ifndef _MANAGER_ADC_H
#define _MANAGER_ADC_H

#include "wiced.h"
#include "string.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"


#include "manager_net.h"
#include "manager_gpio.h"
#include "manager_thread.h"
#include "manager_dct.h"

#include "Adc_i2c.h"
#include <math.h>




typedef struct
{
    float         adc;
    wiced_mutex_t mutex;
} setpoint_t;

typedef struct
{
    adc_reading_t adc_readings[MAX_ADC_READINGS];
    uint16_t              adc_reading_index;
    uint16_t              last_sent_adc_index;
    int16_t               last_sample;
    wiced_mutex_t         mutex;
} temp_data_t;

static temp_data_t         adc_data;
static wiced_timed_event_t adc_timed_event;



extern void wiced_adc_start(void);
extern wiced_result_t wiced_adc_sample(int16_t *);

static wiced_result_t take_adc_reading( void* arg );



float temp_count;
float temp_volt;

/*
 * Takes a adc reading
 */
static wiced_result_t take_adc_reading( void* arg )
{
    UNUSED_PARAMETER(arg);

    wiced_rtos_lock_mutex( &adc_data.mutex );
    if ( wiced_adc_sample(  &adc_data.last_sample ) != WICED_SUCCESS )
    {
        wiced_rtos_unlock_mutex( &adc_data.mutex );
        return WICED_ERROR;
    }

    if ( ( ++adc_data.adc_reading_index ) == MAX_ADC_READINGS )
    {
        adc_data.adc_reading_index = 0;
    }

    wiced_rtos_unlock_mutex( &adc_data.mutex );
    return WICED_SUCCESS;
}



#endif  /* stdbool.h */
