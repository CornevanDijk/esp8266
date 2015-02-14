/*
Some random cgi routines. Used in the LED example and the page that returns the entire
flash as a binary. Also handles the hit counter on the main page.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */


#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "httpd.h"
#include "cgi.h"
#include "io.h"
#include <ip_addr.h>
#include "espmissingincludes.h"

int currLedState;

//Cgi that turns the LED on or off according to the 'led' param in the POST data
int ICACHE_FLASH_ATTR cgiLed(HttpdConnData *connData) {
	int len;
	char buff[1024];
	
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	len=httpdFindArg(connData->postBuff, "led1", buff, sizeof(buff));
	if (len!=0) {
		currLedState=atoi(buff);
		ioLed1(currLedState);
	}
    
    len=httpdFindArg(connData->postBuff, "led2", buff, sizeof(buff));
    if (len!=0) {
        currLedState=atoi(buff);
        ioLed2(currLedState);
    }

	httpdRedirect(connData, "index.tpl");
	return HTTPD_CGI_DONE;
}



//Template code for the led page.
void ICACHE_FLASH_ATTR tplLed(HttpdConnData *connData, char *token, void **arg) {
	char buff[128];
	if (token==NULL) return;

	os_strcpy(buff, "Unknown");
	if (os_strcmp(token, "ledstate1a")==0) {
		if (ioGetLed1()) {
			os_strcpy(buff, "checked");
		} else {
			os_strcpy(buff, "");
		}
	}else
	if (os_strcmp(token, "ledstate1b")==0) {
		if (!ioGetLed1()) {
			os_strcpy(buff, "checked");
		} else {
			os_strcpy(buff, "");
		}
	}else
	if (os_strcmp(token, "ledstate2a")==0) {
		if (ioGetLed2()) {
			os_strcpy(buff, "checked");
		} else {
			os_strcpy(buff, "");
		}
	}else
	if (os_strcmp(token, "ledstate2b")==0) {
		if (!ioGetLed2()) {
			os_strcpy(buff, "checked");
		} else {
			os_strcpy(buff, "");
		}
	}
	httpdSend(connData, buff, -1);
}


//Cgi that reads the SPI flash. Assumes 512KByte flash.
int ICACHE_FLASH_ATTR cgiReadFlash(HttpdConnData *connData) {
	int *pos=(int *)&connData->cgiData;
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	if (*pos==0) {
		os_printf("Start flash download.\n");
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "application/bin");
		httpdEndHeaders(connData);
		*pos=0x40200000;
		return HTTPD_CGI_MORE;
	}
	//Send 1K of flash per call. We will get called again if we haven't sent 512K yet.
	espconn_sent(connData->conn, (uint8 *)(*pos), 1024);
	*pos+=1024;
	if (*pos>=0x40200000+(512*1024)) return HTTPD_CGI_DONE; else return HTTPD_CGI_MORE;
}

