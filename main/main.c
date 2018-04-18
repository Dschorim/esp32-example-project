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
#include "esp_wifi.h"
#include <lwip/sockets.h>
#include <lwip/def.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include <errno.h>
#include "esp_event.h"
#include "esp_event_loop.h"

//for http
#include "freertos/portmacro.h"
#include "freertos/event_groups.h"
#include "tcpip_adapter.h"
#include "lwip/err.h"
#include "string.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "html.c"

#define STATE_NOT_TOUCHED 0
#define STATE_TOUCHED 1

//GPIO4: Touch Channel / Touch Channel 0
//GPIO25: LED
//GPIO34: Potentiometer / ADC1_6

unsigned int low_trigger = 800;
unsigned int high_trigger = 900;
unsigned char state = 0;
unsigned char led_an = 0;
unsigned char led = 255;
unsigned char website_control = 0;

void http_server_netconn_serve(struct netconn *conn)
{
  struct netbuf *inbuf;
  char *buf;
  uint16_t buflen;
  err_t err;

  err = netconn_recv(conn, &inbuf);

  if (err == ERR_OK) {
    netbuf_data(inbuf, (void**)&buf, &buflen);

    if (buflen>=5 &&
        buf[0]=='G' &&
        buf[1]=='E' &&
        buf[2]=='T' &&
        buf[3]==' ' &&
        buf[4]=='/' ) {

      netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);

      if(buf[5]=='o' && !(buf[6]=='?' || buf[5]=='?'|| buf[7]=='?')) {
        if(led_an == 1) led_an=0;
        else led_an=1;
      }
      else if(buf[5]=='c' && !(buf[6]=='?' || buf[5]=='?'|| buf[7]=='?')) {
        if(website_control==1)website_control=0;
        else website_control=1;
      }
      if(buf[6]=='?' || buf[5]=='?'|| buf[7]=='?') {
    	  led = (buf[9]-'0')*100+(buf[10]-'0')*10+(buf[11]-'0');
      }

      netconn_write(conn, http_index_hml1, sizeof(http_index_hml1)-1, NETCONN_NOCOPY);
      if(led_an) netconn_write(conn, http_index_hml2_1, sizeof(http_index_hml2_1)-1, NETCONN_NOCOPY);
      else netconn_write(conn, http_index_hml2_0, sizeof(http_index_hml2_0)-1, NETCONN_NOCOPY);
      netconn_write(conn, http_index_hml3, sizeof(http_index_hml3)-1, NETCONN_NOCOPY);
      if(website_control) netconn_write(conn, http_index_hml4_0, sizeof(http_index_hml4_0)-1, NETCONN_NOCOPY);
      else netconn_write(conn, http_index_hml4_1, sizeof(http_index_hml4_1)-1, NETCONN_NOCOPY);
      netconn_write(conn, http_index_hml5, sizeof(http_index_hml5)-1, NETCONN_NOCOPY);
      http_index_hml6[2] = (led%10)+'0';
      http_index_hml6[1] = (led%100-led%10)/10+'0';
      http_index_hml6[0] = (led%1000-led%100)/100+'0';
      netconn_write(conn, http_index_hml6, sizeof(http_index_hml6)-1, NETCONN_NOCOPY);
      netconn_write(conn, http_index_hml7, sizeof(http_index_hml7)-1, NETCONN_NOCOPY);

    }

  }
  netconn_close(conn);
  netbuf_delete(inbuf);
}

void http_server(void *pvParameters)
{
  struct netconn *conn, *newconn;
  err_t err;
  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, NULL, 80);
  netconn_listen(conn);
  do {
     err = netconn_accept(conn, &newconn);
     if (err == ERR_OK) {
       http_server_netconn_serve(newconn);
       netconn_delete(newconn);
     }
   } while(err == ERR_OK);
   netconn_close(conn);
   netconn_delete(conn);
}

esp_err_t esp32_wifi_eventHandler(void *ctx, system_event_t *event)
{
	return ESP_OK;
}

void setup(){
	ESP_ERROR_CHECK(nvs_flash_init());
	gpio_set_direction(25, GPIO_MODE_OUTPUT);
	touch_pad_init();
	touch_pad_config(0, 0);
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_0db);
	tcpip_adapter_init();
	ESP_ERROR_CHECK( esp_event_loop_init(esp32_wifi_eventHandler, NULL) );
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
	wifi_config_t apConfig = {
	   .ap = {
	      .ssid="ESP32_TESTAP",
	      .ssid_len=0,
	      .password="12345678",
	      .channel=0,
	      .authmode=WIFI_AUTH_WPA2_PSK,
	      .ssid_hidden=0,
	      .max_connection=2,
	      .beacon_interval=100
	   }
	};
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &apConfig) );
	ESP_ERROR_CHECK( esp_wifi_start() );
}

void app_main()
{
	setup();
	xTaskCreatePinnedToCore(&http_server, "http_server", 2048, NULL, 5, NULL, 1);
    uint16_t touch_value;
    uint32_t adc_reading = 0;

    while(1)
    {
    	touch_pad_read(0, &touch_value);
    	adc_reading = adc1_get_voltage(ADC1_CHANNEL_6);
    	if(!website_control)led = 190+adc_reading/63;
    	if(led_an) dac_out_voltage(DAC_CHANNEL_1, led);
    	else dac_out_voltage(DAC_CHANNEL_1, 0);
    	if(touch_value < low_trigger && state == STATE_NOT_TOUCHED)
    	{
    		state = STATE_TOUCHED;
    		if(led_an)led_an=0;
    		else led_an = 1;
    	}

    	if(touch_value > high_trigger && state == STATE_TOUCHED)
    	{
    		state = STATE_NOT_TOUCHED;
    	}
    }
}
