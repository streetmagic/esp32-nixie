/*

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


static const gpio_num_t clock_cathodes[10]={16,17,23,21,18,5,2,27,4,19};
static const  gpio_num_t clock_anodes[4]={26,25,32,33};

time_t now={0};
char strftime_buf[64];
struct tm timeinfo={0};

int display_hours=0;
int display_minutes=0;

#define TEST_MODE 0

#define DYNAMIC_MODE 1
#define DYNAMIC_MODE_PERIOD 10000
#define DYNAMIC_MODE_QUANT 100

#define CATHODE_ON 1
#define CATHODE_OFF 0
#define ANODE_ON 1
#define ANODE_OFF 0

#define CATHODES_CNT 10
#define ANDODES_CNT 4

#define DOT_PIN 22

void nixie_init(void)
{


	gpio_set_direction( DOT_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level( DOT_PIN, ANODE_OFF); 
	gpio_pullup_dis(DOT_PIN);

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



void nixie_set_display(int h,int m, int t_on, int t_off)
{
	int hd=h/10;
	int he=h%10;
	int md=m/10;
	int me=m%10;

	nixie_set_digit(hd,0);
    vTaskDelay(t_on );
	nixie_set_digit(he,1);
	vTaskDelay( t_on );
	nixie_set_digit(md,2);
	vTaskDelay( t_on );
	nixie_set_digit(me,3);
	vTaskDelay( t_on );
}






void clock_display_io_task(void *pvParameter)
{
	for(;;)
	{
		nixie_set_display(display_hours,display_minutes,2,0);	
		esp_task_wdt_reset();
		taskYIELD();
	}

}




void clock_time_get_task(void *pvParameter)
{

	static uint8_t dot_pin_state=0;
	static uint8_t divider=0;

	for(;;)
	{

		vTaskDelay( pdMS_TO_TICKS(500) );

		//timeinfo.tm_hour++;
		//timeinfo.tm_min++;

		divider++;

		if(divider==2)
		{
			
			divider=0;


			#if TEST_MODE

			static int hours=0;
			static int minutes=99;

			if(hours<99) hours++; else hours=0;
			if(minutes>0) minutes--; else minutes=99;

			timeinfo.tm_hour=hours;
			timeinfo.tm_min=minutes;
			
			#else

			time(&now);
			localtime_r(&now, &timeinfo);

			#endif
			
			ESP_LOGI(TAG, " The current time : %d %d %d \n", timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
		}



		if(dot_pin_state)
		{
			dot_pin_state=0;
		}
		else
		{
			dot_pin_state=1;
		}

		gpio_set_level(DOT_PIN, dot_pin_state);
		

		//strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
		//ESP_LOGI(TAG, "The current date/time : %s", strftime_buf);
			
		esp_task_wdt_reset();

		taskYIELD();
	}

}


void clock_render_task(void *pvParameter)
{

	for(;;)
	{
		

		vTaskDelay( pdMS_TO_TICKS(DYNAMIC_MODE_PERIOD) );

		#if DYNAMIC_MODE

		display_hours=0;
		display_minutes=0;

		while ((display_hours<timeinfo.tm_hour)||(display_minutes<timeinfo.tm_min))
		{
			if(display_hours<timeinfo.tm_hour) display_hours++;
			if(display_minutes<timeinfo.tm_min) display_minutes++;
			vTaskDelay( pdMS_TO_TICKS(DYNAMIC_MODE_QUANT) );

		}

		#else

		display_hours=timeinfo.tm_hour;
		display_minutes=timeinfo.tm_min;

		#endif



		esp_task_wdt_reset();

		taskYIELD();
	}

}


void app_main()
{


	nixie_init();
	//xTaskCreatePinnedToCore(&clock_tick_task, "clock_test_task", 2048, NULL, 10, NULL, 1);


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

	//timeinfo.tm_hour=0;
	//timeinfo.tm_min=1;

	setenv("TZ", "MSK-3", 1);
	tzset();


	xTaskCreatePinnedToCore(&clock_display_io_task, "clock_display_io_task", 16384, NULL, 11, NULL, 1);
	xTaskCreatePinnedToCore(&clock_time_get_task, "clock_time_get_task", 16384, NULL, 10, NULL, 1);
	xTaskCreatePinnedToCore(&clock_render_task, "clock_render_task", 16384, NULL, 10, NULL, 1);



	/* clock task */
 // xTaskCreatePinnedToCore(&clock_task, "clock_task", 16384, NULL, CLOCK_TASK_PRIORITY, NULL, 1);

	//xTaskCreatePinnedToCore(&monitoring_task, "monitoring_task", 2048, NULL, 1, NULL, 1);
}
