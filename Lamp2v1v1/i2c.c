/*
 * Copyright 2016, Cypress Semiconductor Corporation or a subsidiary oF
 * Cypress Semiconductor Corporation. All Rights Reserved.
 *
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *
 * I2C ADC Initializing and Measuring functions
 *
 *
 *   Apply a voltage to the ADC pins and view the output.
 *
 */

#include "wiced.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

#define MAX11615_SLAVE_ADDR 		(0x33)
#define MAX11615_SETUP_BYTE 		(0b10110011)
#define NUM_I2C_MESSAGE_RETRIES 	(3)

/******************************************************
 *                   Enumerations
 ******************************************************/

typedef enum
{
	ADC_0 = 0,
	ADC_1,
	ADC_2,
	ADC_3,
	ADC_4,
	ADC_5,
}adc_channel_t;

/******************************************************
 *                 Function Declarations
 ******************************************************/

/******************************************************
 *                 Variables
 ******************************************************/

static wiced_i2c_device_t i2c_device_adc =
{
		.port = WICED_I2C_1,
		.address = MAX11615_SLAVE_ADDR,
		.address_width = I2C_ADDRESS_WIDTH_7BIT,
		.speed_mode = I2C_STANDARD_SPEED_MODE,
};


/******************************************************
 *               Function Definitions
 ******************************************************/
/*
 * Initializes I2C, Probes for ADC and writes ADC Set-up byte
 */
wiced_result_t adc_init( wiced_i2c_device_t* device )
{
	uint8_t setup = MAX11615_SETUP_BYTE;

	/* Initialize I2C */
	WPRINT_APP_INFO( ( "I2C Initialization\n" ) );
	if ( wiced_i2c_init( &i2c_device_adc ) != WICED_SUCCESS )
	{
		WPRINT_APP_INFO( ( "I2C Initialization Failed\n" ) );
		return WICED_ERROR;
	}

	/* Probe I2C bus for ADC */
	WPRINT_APP_INFO( ( "I2C Device Probe\n" ) );
	if( wiced_i2c_probe_device( &i2c_device_adc, NUM_I2C_MESSAGE_RETRIES ) != WICED_TRUE )
	{
		WPRINT_APP_INFO( ( "Failed to connect to ADC device; addr:0x%x\n", i2c_device_adc.address ) );
		return WICED_ERROR;
	}
	WPRINT_APP_INFO( ( "I2C Device Connected at address: 0x%x\n", i2c_device_adc.address ) );

	/* Write ADC Setup Byte */
	WPRINT_APP_INFO( ( "Sending ADC setup byte\r\n" ) );
	if ( wiced_i2c_write( &i2c_device_adc, WICED_I2C_START_FLAG | WICED_I2C_STOP_FLAG, &setup, 1 ) != WICED_SUCCESS )
	{
		WPRINT_APP_INFO( ( "Failed to setup ADC\r\n" ) );
		return WICED_ERROR;
	}

	return WICED_SUCCESS;

}

/*
 * Takes ADC samples through I2C bus
 */
wiced_result_t adc_take_sample( wiced_i2c_device_t* device, adc_channel_t channel, int16_t* output )
{
	uint8_t cmd = 0b01100001; //default [0 11 XXXX 1] [configbyte readonechannel channelselect single-ended]
	cmd |= (channel << 1);
	WPRINT_APP_DEBUG( ("Command Byte: 0x%x\n", cmd) );
	uint8_t data[2]; // 12 bits returned in 2 bytes
	wiced_result_t result;

	/* Write ADC Configuration Byte */
	result = wiced_i2c_write( &i2c_device_adc, WICED_I2C_START_FLAG, &cmd, 1 ); // Send 1 byte, command to start conversion on given channel
	if ( result != WICED_SUCCESS )
	{
		WPRINT_APP_INFO( ( "Failed to send ADC read request.\r\n" ) );
		return WICED_ERROR;
	}

	/* Read ADC Response Bytes */
	result = wiced_i2c_read( &i2c_device_adc, WICED_I2C_REPEATED_START_FLAG|WICED_I2C_STOP_FLAG, data, 2 ); // Read 2 byte response
	if ( result != WICED_SUCCESS )
	{
		WPRINT_APP_INFO( ( "Failed to Read ADC Response.\r\n" ) );
		return WICED_ERROR;
	}

	*output = ((uint16_t)(data[0]&0x0F)<<8 | (uint16_t)data[1]); // discard most significant nibble of first byte
	return WICED_SUCCESS;
}

wiced_result_t wiced_adc_sample(int16_t* output)
{

    return adc_take_sample(&i2c_device_adc,ADC_0,output);
}


/*
 * Initializes ADC
 */
void wiced_adc_start( void )
{
wiced_result_t result;

/* Initialize ADC */
result = adc_init(&i2c_device_adc);
if( result != WICED_SUCCESS )
{
    WPRINT_APP_INFO( ("ADC Initialization failed\n") );
}
}
