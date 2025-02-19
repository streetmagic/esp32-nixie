/*
Copyright (c) 2017-2019 Tony Pottier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

@file main.c
@author Tony Pottier
@brief Entry point for the ESP32 application.
@see https://idyl.io
@see https://github.com/tonyp7/esp32-wifi-manager
*/



#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "mdns.h"
#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/netdb.h"

#include "cJSON.h"


#include "wifi_manager.h"

#include "clock.h"
#include "i2c.h"
#include "ds3231.h"
#include "display.h"
#include "ws2812.h"
#include "webapp.h"

#include "esp_task_wdt.h"

#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
//#include "esp_netif_sntp.h"
#include "lwip/ip_addr.h"
#include "esp_sntp.h"

//static const char* TAG = "MAIN_module";



/* @brief tag used for ESP serial console messages */
static const char TAG[] = "main";

/**
 * @brief RTOS task that periodically prints the heap memory available.
 * @note Pure debug information, should not be ever started on production code
 */
void monitoring_task(void *pvParameter)
{
  for(;;){
    ESP_LOGI(TAG, "free heap: %d",esp_get_free_heap_size());
    vTaskDelay( pdMS_TO_TICKS(20000) );
  }
}

static const gpio_num_t clock_cathodes[10]={16,17,3,21,18,5,2,0,4,19};
static const  gpio_num_t clock_anodes[4]={33,32,25,26};

time_t now;
char strftime_buf[64];
struct tm timeinfo;

#define CATHODE_ON 1
#define CATHODE_OFF 0
#define ANODE_ON 1
#define ANODE_OFF 0

#define CATHODES_CNT 10
#define ANDODES_CNT 4

void nixie_init(void)
{

	for(int i=0; i<CATHODES_CNT; i++)
	{
		gpio_set_direction(clock_cathodes[i], GPIO_MODE_OUTPUT);
		gpio_set_level(clock_cathodes[i], CATHODE_OFF);
	}

	for(int i=0; i<ANDODES_CNT; i++)
	{
		gpio_set_direction(clock_anodes[i], GPIO_MODE_OUTPUT);
		gpio_set_level(clock_anodes[i], ANODE_OFF); 
	}

}



void nixie_set_digit(int digit, int position)
{
	for(int i=0; i<ANDODES_CNT; i++)
	{
		if(position==i)
		{
			gpio_set_level(clock_anodes[i], ANODE_ON); 
		}
		else
		{
			gpio_set_level(clock_anodes[i], ANODE_OFF);
		}

	}

	for(int i=0; i<CATHODES_CNT; i++)
	{
		if(digit==i)
		{
			gpio_set_level(clock_cathodes[i], CATHODE_ON); 
		}
		else
		{
			gpio_set_level(clock_cathodes[i], CATHODE_OFF);
		}

	}
}

void nixie_set_display(int h,int m, int tm)
{
	int hd=h/10;
	int he=h%10;
	int md=m/10;
	int me=m%10;

	nixie_set_digit(hd,3);
    vTaskDelay( pdMS_TO_TICKS(tm) );
	// nixie_set_digit(he,1);
	// vTaskDelay( pdMS_TO_TICKS(tm) );
	// nixie_set_digit(md,2);
	// vTaskDelay( pdMS_TO_TICKS(tm) );
	// nixie_set_digit(me,3);
	// vTaskDelay( pdMS_TO_TICKS(tm) );
	

}



uint32_t clock_ms_tick=0;


void clock_show_task(void *pvParameter)
{
	
	for(;;)
	{
		


		nixie_set_display(timeinfo.tm_hour,timeinfo.tm_min,10);

		//nixie_set_display(cnt/10000,10,10);


		
		esp_task_wdt_reset();
		taskYIELD();
	}

}




void clock_tick_task(void *pvParameter)
{
	for(;;)
	{
		//clock_ms_tick++;

		vTaskDelay( pdMS_TO_TICKS(1000) );

		//if((clock_ms_tick%100)==0)
		//{
			
		//esp_log_write(ESP_LOG_INFO,TAG, "seconds cnt =%d \n ", clock_ms_tick/100);



		time(&now);
		localtime_r(&now, &timeinfo);
		ESP_LOGI(TAG, " The current time : %d %d %d \n", timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);

		//strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
		//ESP_LOGI(TAG, "The current date/time : %s", strftime_buf);
			
		//}

		esp_task_wdt_reset();

		taskYIELD();
	}

}


void app_main()
{


	nixie_init();
	//xTaskCreatePinnedToCore(&clock_tick_task, "clock_test_task", 2048, NULL, 10, NULL, 1);
	 xTaskCreatePinnedToCore(&clock_show_task, "clock_show_task", 16384, NULL, 10, NULL, 1);
	 xTaskCreatePinnedToCore(&clock_tick_task, "clock_tick_task", 16384, NULL, 10, NULL, 1);

	/* GPIO/RMT init for the WS2812 driver */
	//ESP_ERROR_CHECK(ws2812_init());
	
	/* GPIO init for SPI transactions & GPIOs used to control the display */
	//ESP_ERROR_CHECK(display_init());

	/* start the wifi manager */
	wifi_manager_start();
   // webapp_register_handlers();

	/* register cb for internet connectivity */
	wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &clock_notify_sta_got_ip);

	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();

	setenv("TZ", "MSK-3", 1);
	tzset();



	/* clock task */
 // xTaskCreatePinnedToCore(&clock_task, "clock_task", 16384, NULL, CLOCK_TASK_PRIORITY, NULL, 1);

	//xTaskCreatePinnedToCore(&monitoring_task, "monitoring_task", 2048, NULL, 1, NULL, 1);
}
