
/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */


#include "c_types.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "osapi.h"
#include "gpio.h"
#include "espmissingincludes.h"
#include "props.h"

#define LEDGPIO 2
#define BTNGPIO 0

//static ETSTimer resetBtntimer;

void ICACHE_FLASH_ATTR ioLed1(int ena) {
	//gpio_output_set is overkill. ToDo: use better mactos
	if (ena) {
		gpio_output_set((1<<BTNGPIO), 0, (1<<BTNGPIO), 0);
	} else {
		gpio_output_set(0, (1<<BTNGPIO), (1<<BTNGPIO), 0);
	}
}

void ICACHE_FLASH_ATTR ioLed2(int ena) {
    //gpio_output_set is overkill. ToDo: use better mactos
    if (ena) {
        gpio_output_set((1<<LEDGPIO), 0, (1<<LEDGPIO), 0);
    } else {
        gpio_output_set(0, (1<<LEDGPIO), (1<<LEDGPIO), 0);
    }
}

int ioGetLed1(void){
    return GPIO_INPUT_GET(BTNGPIO);
}

int ioGetLed2(void){
    return GPIO_INPUT_GET(LEDGPIO);
}

static int resetCnt=0;
static int resetOn=0;
static ETSTimer resetBtntimer;

static void ICACHE_FLASH_ATTR resetBtnTimerCb(void *arg) {
	if(resetOn == 0 && !GPIO_INPUT_GET(LEDGPIO)){
		os_printf("Soft restart, disable reset.\n");
		os_timer_disarm(&resetBtntimer);
		return;
	}
	if (!GPIO_INPUT_GET(LEDGPIO)) {
		resetCnt++;
		os_printf("Reset counter: %d/6\n", resetCnt);
	}else{
		resetOn++;
		os_printf("Waiting for reset: %d sec\n", (5 - (resetOn/2)));
	}
	if (resetCnt>=6) { //3 sec pressed
		wifi_station_disconnect();
		static struct softap_config apConfig;
		os_strncpy((char*)apConfig.ssid, "native-switcher", 32);
		os_strncpy((char*)apConfig.password, "", 64);
		apConfig.authmode = AUTH_OPEN;
	    apConfig.channel = 7;
	    apConfig.max_connection = 255; // 1?
	    apConfig.ssid_hidden = 0;
		wifi_softap_set_config(&apConfig);
		wifi_set_opmode(0x3); //reset to AP+STA mode
  	    flash_erase_all();
		os_printf("Reset to AP mode. Restarting system...\n");
		system_restart();
		resetCnt=0;
		os_timer_disarm(&resetBtntimer);
	}
	if(resetOn >= 10){
		resetCnt=0;
		os_timer_disarm(&resetBtntimer);
	}

}

void ioInit() {
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
	gpio_output_set(0, 0, 0, (1<<BTNGPIO) | (1<<LEDGPIO));
	os_timer_disarm(&resetBtntimer);
	os_timer_setfn(&resetBtntimer, resetBtnTimerCb, NULL);
	os_timer_arm(&resetBtntimer, 500, 1);
}

