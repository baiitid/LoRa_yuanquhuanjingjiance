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

/*宏定义*/ 
#define START_HEAD 0x55//帧头  
#define CMD_READ   0x01//读数据  
#define ACK_OK     0x00//响应OK  
#define ACK_NONE   0x01//无数据  
#define ACK_ERR    0x02//数据错误  
#define MY_NET_ID  0xD0C2       //网络ID
#define MY_NET_ID  0xD0C2  //网络ID  
#define ADDR_MIN 1         //最小起始地址  
#define ADDR_MAX 2         //最大终结地址


void LoRa_Send( uint8_t *TxBuffer, uint8_t len );
void MyRadioRxDoneProcess( void );
void OLED_InitView(void);
void PlatformInit(void);

uint8_t CheckSum(uint8_t *buf, uint8_t len);  
uint8_t *ExtractCmdframe(uint8_t *buf, uint8_t len, uint8_t CmdStart);  
uint16_t GetHexStr(uint8_t *input, uint16_t len, uint8_t *output);  
void LoRa_DataParse( uint8_t *LoRaRxBuf, uint16_t len );  
void LoRa_GetSensorDataProcess(void); 

void LoRa_SendRead( uint16_t NetId, uint8_t addr )  
{  
    uint8_t TxBuffer[BUFFER_SIZE];  
    TxBuffer[0]=START_HEAD;  
    TxBuffer[1]=CMD_READ;  
    TxBuffer[2]=(uint8_t)(NetId>>8);  
    TxBuffer[3]=(uint8_t)NetId;  
    TxBuffer[4]=addr;  
    TxBuffer[5]=CheckSum((uint8_t *)TxBuffer, 5);  
    Radio.Send( TxBuffer, 6);  
} 

uint8_t CheckSum(uint8_t *buf, uint8_t len)  
{  
    uint8_t temp = 0;  
    while(len--)  
    {  
        temp += *buf;  
        buf++;  
    }  
    return (uint8_t)temp;  
}  

uint8_t *ExtractCmdframe(uint8_t *buf, uint8_t len, uint8_t CmdStart)  
{  
    uint8_t *point = NULL;  
    uint8_t i;  
    for(i=0; i<len; i++)  
    {  
        if(CmdStart == *buf)  
        {  
            point = buf;  
            return point;  
        }  
    buf++;
    }  
    return NULL;  
}  

uint16_t GetHexStr(uint8_t *input, uint16_t len, uint8_t *output)  
{  
    for(uint16_t i=0; i<len; i++)  
    {  
        sprintf((char *)(output+i*3),"%02X ", *input);  
        input++;  
    }  
    return strlen((const char *)output);  
}  

void LoRa_DataParse( uint8_t *LoRaRxBuf, uint16_t len )  
{  
    uint8_t *DestData = NULL;  
#define HEAD_DATA  *DestData     //帧头  
#define CMD_DATA   *(DestData+1) //命令  
#define NETH_DATA  *(DestData+2) //网络ID高字节    
#define NETL_DATA  *(DestData+3) //网络ID低字节    
#define ADDR_DATA  *(DestData+4) //地址    
#define ACK_DATA        *(DestData+5) //响应  
#define LEN_DATA        *(DestData+6) //长度  
#define DATASTAR_DATA   *(DestData+7) //数据域起始  

    DestData = ExtractCmdframe((uint8_t *)LoRaRxBuf, len, START_HEAD);  
    if(DestData != NULL)//检索到数据帧头  
    {  
        if((DestData - LoRaRxBuf) > (len - 6)) return;//数据长度不足构成一帧完整数据  
        if(CMD_DATA != CMD_READ) return;//命令错误   
        if(CheckSum((uint8_t *)DestData, 6+DestData[6]) != (*(DestData+6+(*(DestData+6))))) return;//校验不通过  
        if(((((uint16_t)NETH_DATA)<<8)+NETL_DATA) != MY_NET_ID) return;//网络ID不一致  
        //传感数据显示到OLED屏上  
        char OledBuf[32];  
        memset(OledBuf, ' ', 32);  
        memcpy(OledBuf+1, &DATASTAR_DATA, (LEN_DATA-1)>30?30:(LEN_DATA-1));  
        OLED_ShowString(0,4, (uint8_t *)OledBuf);  
#ifndef TRANSPARENCY  
        //打印接收到的信息到调试助手  
        char output[BUFFER_SIZE*5]={0};  
        memset(output, '\0', BUFFER_SIZE*5);  
        GetHexStr((uint8_t *)LoRaRxBuf, len, (uint8_t *)output);  
        printf("收到%d个字节的LoRa无线数据：%s\r\n", len, (const char *)output);  
        //提取响应数据中的传感数据  
        uint8_t StrBuf[BUFFER_SIZE*5]={0};  
        memset(StrBuf, '\0', BUFFER_SIZE*5);  
        memcpy(StrBuf, &DATASTAR_DATA, LEN_DATA-1);  
        printf("网络ID=0x%04X，源地址=%d\r\n", ((((uint16_t)NETH_DATA)<<8)+NETL_DATA), ADDR_DATA);  
        printf("传感数据：%s\r\n",StrBuf);  
#else  
        USART1_SendStr((uint8_t *)DestData, 7+(*(DestData+6)));//透传  
#endif /*(ENGINEER_DEBUG != false)*/   
    }  
}  


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
        GpioToggle( &Led2 );//收到数据切换亮灯指示  
        LoRa_DataParse( (uint8_t *)RxBuffer, BufferSize );//数据解析
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
    OLED_ShowString(0,0, (uint8_t *)" LoRa Gateway"); 
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
     printf("LoRa Gateway\r\n");  
    Tim3McuInit(1);//定时器初始化，设置定时中断1ms中断一次  
    //IWDG_PrmInit(2048);//独立看门狗初始化，超时设置为2048ms
}
 
void LoRa_ReadSensorProcess(uint8_t AddrMin, uint8_t AddrMax)  
{  
    static uint16_t time = 1000;  
    static uint8_t addr = 1;  
    if(User0Timer_MS > time)  
    {  
            User0Timer_MS = 0;  
            time = randr( 1000, 4000 );//给定一个随机间隔时间，减少同信道通信冲突概率  
#ifndef TRANSPARENCY  
        printf("读取网络ID为0x%04X， 地址为%d的传感节点\r\n", MY_NET_ID, addr);  
#endif  
    //显示屏提示轮询地址和网络号  
    char StrBuf[32];  
    memset(StrBuf, '\0', 32);  
        sprintf(StrBuf, " ID:%04X,Addr:%d", MY_NET_ID, addr);  
        OLED_ShowString(0,2, (uint8_t *)StrBuf);  
        //清除显示屏第3、4行的内容  
        memset(StrBuf, ' ', 32);  
                OLED_ShowString(0,4, (uint8_t *)StrBuf);  
        LoRa_SendRead( MY_NET_ID, addr++ );  
            if(addr > AddrMax)  
        {  
            addr = AddrMin;  
        }  
        GpioToggle( &Led1 );//发送数据切换亮灯指示  
    }  
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
        MyRadioRxDoneProcess();//LoRa无线射频接收数据处理进程  
        LoRa_ReadSensorProcess(ADDR_MIN, ADDR_MAX);  //轮询进程
    }
}
