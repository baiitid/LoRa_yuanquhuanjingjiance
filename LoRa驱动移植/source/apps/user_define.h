#ifndef _USER_DEFINE_H_
#define _USER_DEFINE_H_

#include <stdbool.h>

//LORA模块相关
#define USE_SX1276_RESET 			false 	//没有使用该引脚
#define USE_SX1276_DIO2_TO_DIO5 	false 	//没有使用SX_1276的DIO2到DIO5的引脚
#define	USE_SX1276_RXTX 			false 	//没有使用该引脚
#define USE_RADIO_ANT_SWITCH 		false

//LORA模块SX1276/1278寄存器配置相关

//电池相关
#define USE_BATTERY 				false

//调试仿真相关
#define ENGINEER_DEBUG 				false//true	//调试信息开关，true时会打印printf到串口


#endif //_USER_DEFINE_H_
