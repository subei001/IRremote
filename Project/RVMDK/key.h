#ifndef _key_H_
#define _key_H_

#include "stm32f10x.h"

void Matrixkey_Init(void);
unsigned char Get_KeyValue(void);
void treat_key_value(unsigned char key_value);






#endif
