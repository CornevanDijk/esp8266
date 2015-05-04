/*
 * props.h
 *
 *  Created on: 1 maj 2015
 *      Author: pkarpins
 */

#ifndef USER_PROPS_H_
#define USER_PROPS_H_

int flash_key_value_set(const char *key,const char *value);
int flash_key_value_get(char *key,char *value);
void flash_erase_all();


#endif /* USER_PROPS_H_ */
