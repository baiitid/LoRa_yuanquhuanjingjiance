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

/*�궨��*/ 
#define START_HEAD 0x55//֡ͷ  
#define CMD_READ   0x01//������  
#define ACK_OK     0x00//��ӦOK  
#define ACK_NONE   0x01//������  
#define ACK_ERR    0x02//���ݴ���  
#define MY_NET_ID  0xD0C2       //����ID
#define MY_NET_ID  0xD0C2  //����ID  
#define ADDR_MIN 1         //��С��ʼ��ַ  
#define ADDR_MAX 2         //����ս��ַ


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
#define HEAD_DATA  *DestData     //֡ͷ  
#define CMD_DATA   *(DestData+1) //����  
#define NETH_DATA  *(DestData+2) //����ID���ֽ�    
#define NETL_DATA  *(DestData+3) //����ID���ֽ�    
#define ADDR_DATA  *(DestData+4) //��ַ    
#define ACK_DATA        *(DestData+5) //��Ӧ  
#define LEN_DATA        *(DestData+6) //����  
#define DATASTAR_DATA   *(DestData+7) //��������ʼ  

    DestData = ExtractCmdframe((uint8_t *)LoRaRxBuf, len, START_HEAD);  
    if(DestData != NULL)//����������֡ͷ  
    {  
        if((DestData - LoRaRxBuf) > (len - 6)) return;//���ݳ��Ȳ��㹹��һ֡��������  
        if(CMD_DATA != CMD_READ) return;//�������   
        if(CheckSum((uint8_t *)DestData, 6+DestData[6]) != (*(DestData+6+(*(DestData+6))))) return;//У�鲻ͨ��  
        if(((((uint16_t)NETH_DATA)<<8)+NETL_DATA) != MY_NET_ID) return;//����ID��һ��  
        //����������ʾ��OLED����  
        char OledBuf[32];  
        memset(OledBuf, ' ', 32);  
        memcpy(OledBuf+1, &DATASTAR_DATA, (LEN_DATA-1)>30?30:(LEN_DATA-1));  
        OLED_ShowString(0,4, (uint8_t *)OledBuf);  
#ifndef TRANSPARENCY  
        //��ӡ���յ�����Ϣ����������  
        char output[BUFFER_SIZE*5]={0};  
        memset(output, '\0', BUFFER_SIZE*5);  
        GetHexStr((uint8_t *)LoRaRxBuf, len, (uint8_t *)output);  
        printf("�յ�%d���ֽڵ�LoRa�������ݣ�%s\r\n", len, (const char *)output);  
        //��ȡ��Ӧ�����еĴ�������  
        uint8_t StrBuf[BUFFER_SIZE*5]={0};  
        memset(StrBuf, '\0', BUFFER_SIZE*5);  
        memcpy(StrBuf, &DATASTAR_DATA, LEN_DATA-1);  
        printf("����ID=0x%04X��Դ��ַ=%d\r\n", ((((uint16_t)NETH_DATA)<<8)+NETL_DATA), ADDR_DATA);  
        printf("�������ݣ�%s\r\n",StrBuf);  
#else  
        USART1_SendStr((uint8_t *)DestData, 7+(*(DestData+6)));//͸��  
#endif /*(ENGINEER_DEBUG != false)*/   
    }  
}  


/**********************************************************************************************
*������void LoRa_Send( uint8_t *TxBuffer, uint8_t len )
*���ܣ�����LoRa��������
*���룺
*       uint8_t *TxBuffer, ����������
*       uint8_t len, ���������ݳ���
*�������
*���أ���
*����˵������
**********************************************************************************************/
void LoRa_Send( uint8_t *TxBuffer, uint8_t len )
{
    Radio.Send( TxBuffer, len);
}

/**********************************************************************************************
*������void MyRadioRxDoneProcess( void )
*���ܣ�����ģ�����ݽ�����ɴ�����̺���
*���룺��
*�������
*���أ���
*����˵�������յ����������ݱ�����RxBuffer�У�BufferSizeΪ���յ����������ݳ���
**********************************************************************************************/
void MyRadioRxDoneProcess( void )
{
    uint16_t BufferSize = 0;
    uint8_t RxBuffer[BUFFER_SIZE];

    BufferSize = ReadRadioRxBuffer( (uint8_t *)RxBuffer );
    if(BufferSize>0)
    {
        //�û��ڴ˴���ӽ������ݴ����ܵĴ���
        GpioToggle( &Led2 );//�յ������л�����ָʾ  
        LoRa_DataParse( (uint8_t *)RxBuffer, BufferSize );//���ݽ���
    }
}

/**********************************************************************************************
*������void OLED_InitView()
*���ܣ�LoraҺ����ʼ��
*���룺��
*�������
*����˵������
**********************************************************************************************/
void OLED_InitView(void)
{
    OLED_Clear();
    OLED_ShowString(0,0, (uint8_t *)" LoRa Gateway"); 
}

/**********************************************************************************************
*������void PlatformInit( void )
*���ܣ�ƽ̨��ʼ��
*���룺��
*�������
*����˵������
**********************************************************************************************/
void PlatformInit(void)
{
    // ������ƽ̨��ʼ��
    BoardInitMcu();
    BoardInitPeriph();

    // �������豸��ʼ��
    OLED_Init();//Һ����ʼ��
    USART1_Init(115200);//����1��ʼ��
    OLED_Clear();
    OLED_InitView();//OLED��Ļ��ʾ��ʼ��Ϣ
    
    printf("�´�½���� LoRa \r\n");

    //Loraģ���ʼ��
    NS_RadioInit( (uint32_t) RF_PING_PONG_FREQUENCY, (int8_t) TX_OUTPUT_POWER, (uint32_t) TX_TIMEOUT_VALUE, (uint32_t) RX_TIMEOUT_VALUE );
    
    //�����·�����û���ʼ������
     printf("LoRa Gateway\r\n");  
    Tim3McuInit(1);//��ʱ����ʼ�������ö�ʱ�ж�1ms�ж�һ��  
    //IWDG_PrmInit(2048);//�������Ź���ʼ������ʱ����Ϊ2048ms
}
 
void LoRa_ReadSensorProcess(uint8_t AddrMin, uint8_t AddrMax)  
{  
    static uint16_t time = 1000;  
    static uint8_t addr = 1;  
    if(User0Timer_MS > time)  
    {  
            User0Timer_MS = 0;  
            time = randr( 1000, 4000 );//����һ��������ʱ�䣬����ͬ�ŵ�ͨ�ų�ͻ����  
#ifndef TRANSPARENCY  
        printf("��ȡ����IDΪ0x%04X�� ��ַΪ%d�Ĵ��нڵ�\r\n", MY_NET_ID, addr);  
#endif  
    //��ʾ����ʾ��ѯ��ַ�������  
    char StrBuf[32];  
    memset(StrBuf, '\0', 32);  
        sprintf(StrBuf, " ID:%04X,Addr:%d", MY_NET_ID, addr);  
        OLED_ShowString(0,2, (uint8_t *)StrBuf);  
        //�����ʾ����3��4�е�����  
        memset(StrBuf, ' ', 32);  
                OLED_ShowString(0,4, (uint8_t *)StrBuf);  
        LoRa_SendRead( MY_NET_ID, addr++ );  
            if(addr > AddrMax)  
        {  
            addr = AddrMin;  
        }  
        GpioToggle( &Led1 );//���������л�����ָʾ  
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
        //IWDG_PrmRefresh( );//ι�������Ź�
        //�����·�����û����ܴ���
        MyRadioRxDoneProcess();//LoRa������Ƶ�������ݴ������  
        LoRa_ReadSensorProcess(ADDR_MIN, ADDR_MAX);  //��ѯ����
    }
}
