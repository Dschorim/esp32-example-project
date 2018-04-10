#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "driver/touch_pad.h"
#include "driver/dac.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define STATE_NOT_TOUCHED 0
#define STATE_TOUCHED 1

//GPIO4: Touch Channel / Touch Channel 0
//GPIO25: LED
//GPIO34: Poti / ADC1_6

static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;

unsigned int low_trigger = 850;
unsigned int high_trigger = 950;
unsigned char state = 0;
unsigned char led_an = 0;
unsigned char led = 255;

void setup(){
	gpio_set_direction(25, GPIO_MODE_OUTPUT);
	touch_pad_init();
	touch_pad_config(0, 0);
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(channel, atten);
}

void app_main()
{
	setup();
    uint16_t touch_value;
    uint32_t adc_reading = 0;

    while(1)
    {
    	touch_pad_read(0, &touch_value);
    	adc_reading = adc1_get_raw(ADC_CHANNEL_6);
    	led = adc_reading/16;
    	if(touch_value < low_trigger && state == STATE_NOT_TOUCHED)
    	{
    		state = STATE_TOUCHED;
    		led_an = ~led_an;
    		if(led_an) dac_output_voltage(DAC_CHANNEL_1, led);
    		else dac_output_voltage(DAC_CHANNEL_1, 0);
    	}

    	if(touch_value > high_trigger && state == STATE_TOUCHED)
    	{
    		state = STATE_NOT_TOUCHED;
    	}
    	//printf("Value: %5d\n",touch_value);
    	//printf("State: %d\n",state);
    	//vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
