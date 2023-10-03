/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  */
#include <string.h>
#include <stdio.h>
#include "board.h"
#include "radio.h"
#include "NS_Radio.h"
#include "sht3x.h"


void LoRa_Send( uint8_t *TxBuffer, uint8_t len );
void MyRadioRxDoneProcess( void );
void OLED_InitView(void);
void PlatformInit(void);

/**********************************************************************************************
*函数：void LoRa_Send( uint8_t *TxBuffer, uint8_t len )
*功能：发送LoRa无线数据
*输入：
*       uint8_t *TxBuffer, 待发送数据
*       uint8_t len, 待发送数据长度
*输出：无
*返回：无
*特殊说明：无
**********************************************************************************************/
void LoRa_Send( uint8_t *TxBuffer, uint8_t len )
{
    Radio.Send( TxBuffer, len);
}

/**********************************************************************************************
*函数：void MyRadioRxDoneProcess( void )
*功能：无线模块数据接收完成处理进程函数
*输入：无
*输出：无
*返回：无
*特殊说明：接收到的无线数据保存在RxBuffer中，BufferSize为接收到的无线数据长度
**********************************************************************************************/
void MyRadioRxDoneProcess( void )
{
    uint16_t BufferSize = 0;
    uint8_t RxBuffer[BUFFER_SIZE];

    BufferSize = ReadRadioRxBuffer( (uint8_t *)RxBuffer );
    if(BufferSize>0)
    {
        //用户在此处添加接收数据处理功能的代码
        ;
        
        
    }
}

/**********************************************************************************************
*函数：void OLED_InitView()
*功能：Lora液晶初始化
*输入：无
*输出：无
*特殊说明：无
**********************************************************************************************/
void OLED_InitView(void)
{
    OLED_Clear();
    OLED_ShowString(0,0, (uint8_t *)" Newland Edu");
}

/**********************************************************************************************
*函数：void PlatformInit( void )
*功能：平台初始化
*输入：无
*输出：无
*特殊说明：无
**********************************************************************************************/
void PlatformInit(void)
{
    // 开发板平台初始化
    BoardInitMcu();
    BoardInitPeriph();

    // 开发板设备初始化
    OLED_Init();//液晶初始化
    USART1_Init(115200);//串口1初始化
    OLED_Clear();
    OLED_InitView();//OLED屏幕显示初始信息
    
    printf("新大陆教育 LoRa \r\n");

    //Lora模块初始化
    NS_RadioInit( (uint32_t) RF_PING_PONG_FREQUENCY, (int8_t) TX_OUTPUT_POWER, (uint32_t) TX_TIMEOUT_VALUE, (uint32_t) RX_TIMEOUT_VALUE );
    
    //请在下方添加用户初始化代码
    
    
    
    //IWDG_PrmInit(2048);//独立看门狗初始化，超时设置为2048ms
}


/**
 * Main application entry point.
 */
int main( void )
{
    PlatformInit();

    while( 1 )
    {
        //IWDG_PrmRefresh( );//喂独立看门狗
        //请在下方添加用户功能代码
        
        
    }
}
