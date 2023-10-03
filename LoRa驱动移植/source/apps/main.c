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
        ;
        
        
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
    OLED_ShowString(0,0, (uint8_t *)" Newland Edu");
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
    
    
    
    //IWDG_PrmInit(2048);//�������Ź���ʼ������ʱ����Ϊ2048ms
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
        
        
    }
}
