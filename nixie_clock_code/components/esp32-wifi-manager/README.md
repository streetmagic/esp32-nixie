# What is esp32-wifi-manager?

### Build status [![Build Status](https://travis-ci.com/tonyp7/esp32-wifi-manager.svg?branch=master)](https://travis-ci.com/tonyp7/esp32-wifi-manager)

*esp32-wifi-manager* is a pure C esp-idf component for ESP32 that enables easy management of wifi networks through a web portal.

*esp32-wifi-manager* is is an all in one wifi scanner, http server & dns daemon living in the least amount of RAM possible.

*esp32-wifi-manager* will automatically attempt to re-connect to a previously saved network on boot, and if it cannot find a saved wifi it will start its own access point through which you can manage and connect to wifi networks. Upon a succesful connection, the software will shutdown the access point automatically after some time (1 minute by default).

*esp32-wifi-manager* compiles with esp-idf 4.1 and above. See [Getting Started](#getting-started) to guide you through your first setup.

# Content
 - [Demo](#demo)
 - [Look And Feel](#look-and-feel)
 - [Getting Started](#getting-started)
   - [Requirements](#requirements)
   - [Hello World](#hello-world)
   - [Configuring the Wifi Manager](#configuring-the-wifi-manager)
 - [Adding esp32-wifi-manager to your code](#adding-esp32-wifi-manager-to-your-code)
   - [Interacting with the manager](#interacting-with-the-manager)
   - [Interacting with the http server](#interacting-with-the-http-server)
 - [License](#license)
   

# Demo
[![esp32-wifi-manager demo](http://img.youtube.com/vi/hxlZi15bym4/0.jpg)](http://www.youtube.com/watch?v=hxlZi15bym4)

# Look and Feel
![esp32-wifi-manager on an mobile device](https://idyl.io/wp-content/uploads/2017/11/esp32-wifi-manager-password.png "esp32-wifi-manager") ![esp32-wifi-manager on an mobile device](https://idyl.io/wp-content/uploads/2017/11/esp32-wifi-manager-connected-to.png "esp32-wifi-manager")

# Getting Started

## Requirements

To get you started, esp32-wifi-manager needs:

- esp-idf 4.1 and up
- esp32 or esp32-s2

Due to breaking changes in esp-idf 4.1, most notably the complete revision on how the event wifi loop works and how the tcpip library was deprecated in favour of a newer library (esp_netif), esp32-wifi-manager will not work with older releases of espressif's frameworks

## Hello World

Clone the repository where you want it to be. If you are unfamiliar with Git, you can use Github Desktop on Windows:

```bash 
git clone https://github.com/tonyp7/esp32-wifi-manager.git
```

Navigate under the included example:

```bash
cd esp32-wifi-manager/examples/default_demo
```

Compile the code and load it on your esp32:

```bash
idf.py build flash monitor
```

_Note: while it is encouraged to use the newer build system with idf.py and cmake, esp32-wifi-manager still supports the legacy build system. If you are using make on Linux or make using MSYS2 on Windows, you can still use "make build flash monitor" if you prefer_

Now, using any wifi capable device, you will see a new wifi access point named *esp32*. Connect to it using the default password *esp32pwd*. If the captive portal does not pop up on your device, you can access the wifi manager at its default IP address: http://10.10.0.1.

## Configuring the Wifi Manager

esp32-wifi-manager can be configured without touching its code. At the project level use:

```bash
idf.py menuconfig
```

Navigate in "Component config" then pick "Wifi Manager Configuration". You will be greeted by the following screen:

![esp32-wifi-manager-menuconfig](https://idyl.io/wp-content/uploads/2020/07/esp32-wifi-manager-menuconfig-800px.png "menuconfig screen")

You can change the ssid and password of the access point at your convenience, but it is highly recommended to keep default values. Your password should be between 8 and 63 characters long, to comply with the WPA2 standard.

You can also change the values for various timers, for instance how long it takes for the access point to shutdown once a connection is established (default: 60000). While it could be tempting to set this timer to 0, just be warned that in that case the user will never get the feedback that a connection is succesful. Shutting down the AP will instantly kill the current navigating session on the captive portal.

# Adding esp32-wifi-manager to your code

In order to use esp32-wifi-manager effectively in your esp-idf projects, copy the whole esp32-wifi-manager repository (or git clone) into a components subfolder.

Your project should look like this:

  - project_folder
    - build
    - components
      - esp32-wifi-manager
    - main
      - main.c

Under eclipse, this is what a typical project looks like:

![eclipse project with esp32-wifi-manager](https://idyl.io/wp-content/uploads/2020/07/eclipse-idf-project.png "eclipse project with esp32-wifi-manager")

Once this is done, you need to edit the CMakeLists.txt file at the root of your project to register the components folder. This is done by adding the following line:

```cmake
set(EXTRA_COMPONENTS_DIRS components/)
```

A typical CmakeLists.txt file should look like this:

```cmake
cmake_minimum_required(VERSION 3.5)
set(EXTRA_COMPONENT_DIRS components/)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(name_of_your_project)
```

If you are using the old build system with make instead, you should edit the Makefile instead such as:

```make
PROJECT_NAME := name_of_your_project
EXTRA_COMPONENT_DIRS := components/
include $(IDF_PATH)/make/project.mk
```

Once this is done, you can now in your user code add the header:

```c
#include "wifi_manager.h"
```

All you need to do now is to call wifi_manager_start(); in your code. See [examples/default_demo](examples/default_demo) if you are uncertain.


## Interacting with the manager

Ther are effectively three different ways you can embed esp32-wifi-manager with your code:
* Just forget about it and poll in your code for wifi connectivity status
* Use event callbacks
* Modify esp32-wifi-manager code directly to fit your needs

**Event callbacks** are the cleanest way to use the wifi manager and that's the recommended way to do it. A typical use-case would be to get notified when wifi manager finally gets a connection an access point. In order to do this you can simply define a callback function:

```c
void cb_connection_ok(void *pvParameter){
	ESP_LOGI(TAG, "I have a connection!");
}
```

Then just register it by calling:

```c
wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &cb_connection_ok);
```

That's it! Now everytime the event is triggered it will call this function. The [examples/default_demo](examples/default_demo) contains sample code using callbacks.

### List of events

The list of possible events you can add a callback to are defined by message_code_t in wifi_manager.h. They are as following:

* WM_ORDER_START_HTTP_SERVER
* WM_ORDER_STOP_HTTP_SERVER
* WM_ORDER_START_DNS_SERVICE
* WM_ORDER_STOP_DNS_SERVICE
* WM_ORDER_START_WIFI_SCAN
* WM_ORDER_LOAD_AND_RESTORE_STA
* WM_ORDER_CONNECT_STA
* WM_ORDER_DISCONNECT_STA
* WM_ORDER_START_AP
* WM_EVENT_STA_DISCONNECTED
* WM_EVENT_SCAN_DONE
* WM_EVENT_STA_GOT_IP
* WM_ORDER_STOP_AP

In practice, keeping track of WM_EVENT_STA_GOT_IP and WM_EVENT_STA_DISCONNECTED is key to know whether or not your esp32 has a connection. The other messages can mostly be ignored in a typical applicationn using esp32-wifi-manager.

## Interacting with the http server

Because esp32-wifi-manager spawns its own http server, you might want to extend this server to serve your own pages in your application. It is possible to do so by registering your own URL handler using the standard esp_http_server signature:

```c
esp_err_t my_custom_handler(httpd_req_t *req){
```

And then registering the handler by doing

```c
http_app_set_handler_hook(HTTP_GET, &my_custom_handler);
```

The [examples/http_hook](examples/http_hook) contains an example where a web page is registered at /helloworld

# License
*esp32-wifi-manager* is MIT licensed. As such, it can be included in any project, commercial or not, as long as you retain original copyright. Please make sure to read the license file.
