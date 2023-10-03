/**********************************************************************************************************
*
*
*
*
**********************************************************************************************************/
#include <string.h>
#include "board.h"
#include "radio.h"
#include "NS_Radio.h"

static uint16_t LoRaBufferSize = BUFFER_SIZE;
static uint8_t LoRaBuffer[BUFFER_SIZE];
static int8_t RssiValue = 0;
static int8_t SnrValue = 0;

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/**********************************************************************************************
*函数：void NS_RadioEventsInit( void )
*功能：对射频模块事件回调函数进行初始化
*输入：无
*输出：无
*特殊说明：无
**********************************************************************************************/
void NS_RadioEventsInit( void )
{
    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;

    Radio.Init( &RadioEvents );
}


/**********************************************************************************************
*函数：void NS_RadioInit( uint32_t freq, int8_t power, uint32_t txTimeout )
*功能：对射频模块发送、接收参数进行初始化
*输入：
*			freq载波频率，建议433MHz附近频率最佳
*			power发射功率，最大20
*			txTimeout发送超时时间
*输出：无
*特殊说明：无
**********************************************************************************************/
void NS_RadioSetTxRxConfig( uint32_t freq, int8_t power, uint32_t txTimeout )
{
    // Radio initialization
    Radio.SetChannel( freq );
    /*!
     * \brief Sets the transmission parameters
     *
     * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]
     * \param [IN] power        Sets the output power [dBm]
     * \param [IN] fdev         Sets the frequency deviation (FSK only)
     *                          FSK : [Hz]
     *                          LoRa: 0
     * \param [IN] bandwidth    Sets the bandwidth (LoRa only)
     *                          FSK : 0
     *                          LoRa: [0: 125 kHz, 1: 250 kHz,
     *                                 2: 500 kHz, 3: Reserved]
     * \param [IN] datarate     Sets the Datarate
     *                          FSK : 600..300000 bits/s
     *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
     *                                10: 1024, 11: 2048, 12: 4096  chips]
     * \param [IN] coderate     Sets the coding rate (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
     * \param [IN] preambleLen  Sets the preamble length
     *                          FSK : Number of bytes
     *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
     * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
     * \param [IN] crcOn        Enables disables the CRC [0: OFF, 1: ON]
     * \param [IN] FreqHopOn    Enables disables the intra-packet frequency hopping
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: OFF, 1: ON]
     * \param [IN] HopPeriod    Number of symbols between each hop
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: Number of symbols
     * \param [IN] iqInverted   Inverts IQ signals (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: not inverted, 1: inverted]
     * \param [IN] timeout      Transmission timeout [ms]
     */
    Radio.SetTxConfig( MODEM_LORA, power, 0, LORA_BANDWIDTH,
                       LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                       LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                       true, 0, 0, LORA_IQ_INVERSION_ON, txTimeout/*TX_TIMEOUT_VALUE*/ );
    /*!
     * \brief Sets the reception parameters
     *
     * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]
     * \param [IN] bandwidth    Sets the bandwidth
     *                          FSK : >= 2600 and <= 250000 Hz
     *                          LoRa: [0: 125 kHz, 1: 250 kHz,
     *                                 2: 500 kHz, 3: Reserved]
     * \param [IN] datarate     Sets the Datarate
     *                          FSK : 600..300000 bits/s
     *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
     *                                10: 1024, 11: 2048, 12: 4096  chips]
     * \param [IN] coderate     Sets the coding rate (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
     * \param [IN] bandwidthAfc Sets the AFC Bandwidth (FSK only)
     *                          FSK : >= 2600 and <= 250000 Hz
     *                          LoRa: N/A ( set to 0 )
     * \param [IN] preambleLen  Sets the Preamble length
     *                          FSK : Number of bytes
     *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
     * \param [IN] symbTimeout  Sets the RxSingle timeout value
     *                          FSK : timeout in number of bytes
     *                          LoRa: timeout in symbols
     * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
     * \param [IN] payloadLen   Sets payload length when fixed length is used
     * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
     * \param [IN] FreqHopOn    Enables disables the intra-packet frequency hopping
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: OFF, 1: ON]
     * \param [IN] HopPeriod    Number of symbols between each hop
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: Number of symbols
     * \param [IN] iqInverted   Inverts IQ signals (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: not inverted, 1: inverted]
     * \param [IN] rxContinuous Sets the reception in continuous mode
     *                          [false: single mode, true: continuous mode]
     */
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                       LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                       LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                       0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
}

/**********************************************************************************************
*函数：void NS_RadioInit( uint32_t freq, int8_t power, uint32_t txTimeout, uint32_t rxTimeout)
*功能：对射频模块发送、接收参数进行初始化
*输入：
*			freq载波频率，建议433MHz附近频率最佳
*			power发射功率，最大20
*			txTimeout发送超时时间
*			uint32_t rxTimeout 接收超时时间
*输出：无
*特殊说明：无
**********************************************************************************************/
void NS_RadioInit( uint32_t freq, int8_t power, uint32_t txTimeout, uint32_t rxTimeout)
{
    NS_RadioEventsInit();//无线射频模块注册事件回调函数
    NS_RadioSetTxRxConfig( freq, power, txTimeout );
    Radio.Rx( rxTimeout );//设置LoRa模块为接收模式，同时设置超时时间，必许步骤
}

/**********************************************************************************************
*函数：uint16_t ReadRadioRxBuffer( uint8_t *payload)
*功能：读取射频接收缓存信息，包括读取接收到的数据长度和缓存数据。
*输入：无
*输出：uint8_t *payload, 指针，指向要用于存放接收缓存的地址
*返回：返回接收到的数据长度
*特殊说明：每调用一次ReadRadioRxBuffer( )函数，LoRaBufferSize会被置0.
**********************************************************************************************/
uint16_t ReadRadioRxBuffer( uint8_t *payload)
{
    uint16_t LengthTemp;
    LengthTemp = LoRaBufferSize;
    if(LengthTemp <= 0)
    {
        return 0;
    }
    LoRaBufferSize = 0;
//    payload = LoRaBuffer;
    memcpy( payload, LoRaBuffer, LengthTemp );
    return LengthTemp;
}

/**********************************************************************************************
*函数：void ReadRadioRssiSnr( int16_t *rssi, int8_t *snr )
*功能：读取射频信号强度和信噪比
*输入：无
*输出：int16_t *rssi,信号强度 int8_t *snr信噪比
*返回：无
*特殊说明：无
**********************************************************************************************/
void ReadRadioRssiSnr( int16_t *rssi, int8_t *snr )
{
    *rssi = RssiValue;
    *snr = SnrValue;
}

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void )
{
    Radio.Sleep( );
    Radio.Rx( RX_TIMEOUT_VALUE );
}

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.Sleep( );
    LoRaBufferSize = size;
    memcpy( LoRaBuffer, payload, LoRaBufferSize );
    RssiValue = rssi;
    SnrValue = snr;
//    printf( "Rx=%s\r\nRssiValue=%d\r\nSnrValue=%d\r\n",LoRaBuffer,RssiValue,SnrValue );
    Radio.Rx( RX_TIMEOUT_VALUE );
}

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void )
{
    Radio.Sleep( );
    Radio.Rx( RX_TIMEOUT_VALUE );
}

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void )
{
    Radio.Sleep( );
    Radio.Rx( RX_TIMEOUT_VALUE );
}

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void )
{
    Radio.Sleep( );
    Radio.Rx( RX_TIMEOUT_VALUE );
}
