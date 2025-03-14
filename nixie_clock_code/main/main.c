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













void app_main()
{


	
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





	/* clock task */
    xTaskCreatePinnedToCore(&clock_task, "clock_task", 16384, NULL, CLOCK_TASK_PRIORITY, NULL, 1);

	xTaskCreatePinnedToCore(&monitoring_task, "monitoring_task", 2048, NULL, 1, NULL, 1);
}
