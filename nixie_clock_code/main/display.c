/*


@file display.c
@author Danila K

Contains functions to control the nixie clock display over nogodryg (dynamic indication)

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

static const gpio_num_t clock_cathodes[10]={C0_PIN,C1_PIN,C2_PIN,C3_PIN,C4_PIN,C5_PIN,C6_PIN,C7_PIN,C8_PIN,C9_PIN};
static const  gpio_num_t clock_anodes[4]={A0_PIN,A1_PIN,A2_PIN,A3_PIN};


static int display_hours=0;
static int display_minutes=0;
static bool display_state=1;




void display_io_off(void)
{
	for(int i=0; i<CATHODES_CNT; i++)
	{
		gpio_set_level(clock_cathodes[i], CATHODE_OFF);
	}

	for(int i=0; i<ANDODES_CNT; i++)
	{
		gpio_set_level(clock_anodes[i], ANODE_OFF); 
	}
}


void display_set_digit(int digit, int position)
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



void display_set_io(int h,int m, int t_on, int t_off)
{
	int hd=h/10;
	int he=h%10;
	int md=m/10;
	int me=m%10;

	display_set_digit(hd,0);
    vTaskDelay(t_on );
	display_set_digit(he,1);
	vTaskDelay( t_on );
	display_set_digit(md,2);
	vTaskDelay( t_on );
	display_set_digit(me,3);
	vTaskDelay( t_on );
}



void display_on(void)
{
	display_state=1;

}

void display_off(void)
{
	display_state=0;

}
void display_io_task(void *pvParameter)
{
	for(;;)
	{
		if(display_state)
		{
			display_set_io(display_hours,display_minutes,DYNAMIC_INDICATION_TIME_MS,0);	
		}
		else
		{
			display_io_off();
			vTaskDelay(10);
		}

		esp_task_wdt_reset();
		taskYIELD();
	}

}


void display_init(void)
{


	gpio_set_direction( DOT_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level( DOT_PIN, ANODE_OFF); 
	gpio_pullup_dis(DOT_PIN);

	for(int i=0; i<CATHODES_CNT; i++)
	{
		gpio_set_direction(clock_cathodes[i], GPIO_MODE_OUTPUT);
		
	}

	for(int i=0; i<ANDODES_CNT; i++)
	{
		gpio_set_direction(clock_anodes[i], GPIO_MODE_OUTPUT);
		
	}

	display_io_off();

	xTaskCreatePinnedToCore(&display_io_task, "display_io_task", 16384, NULL, 11, NULL, 1);

}





void display_set(int h,int m)
{
	display_hours=h;
	display_minutes=m;


}

void display_set_dots(bool state)
{
	gpio_set_level(DOT_PIN, state);
}





