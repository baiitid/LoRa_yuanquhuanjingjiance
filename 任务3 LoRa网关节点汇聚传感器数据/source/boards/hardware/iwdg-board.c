/************************************************************************************************************************************************
*
*
*
*
*
*
*
*************************************************************************************************************************************************/

#include "stm32l1xx_hal.h"
#include "stm32l1xx_hal_iwdg.h"
#include "iwdg-board.h"

/* IWDG handlers declaration */
IWDG_HandleTypeDef IwdgHandle;

/*******************************************************************
*函数：void IWDG_PrmInit(void)
*功能：独立看门狗参数初始化
*输入：uint16_t IwdgMs,看门狗超时时间，单位:ms毫秒
*输出：无
*返回：无
*特殊说明：由于LSI晶振32768Hz被32分频，重载寄存器最大值0xFFF，计算可得
IwdgMs的最大值不得超过4000ms
*******************************************************************/
void IWDG_PrmInit(uint16_t IwdgMs)
{
    /*##-1- Check if the system has resumed from IWDG reset ####################*/
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET)
    {
        /* IWDGRST flag set*/
        printf("从独立看门狗重启\r\n");
        /* Clear reset flags */
        __HAL_RCC_CLEAR_RESET_FLAGS();
    }
    else
    {
        /* IWDGRST flag is not set*/
        ;
    }

    /*##-2- Get the LSI frequency: TIM10 is used to measure the LSI frequency ###*/
#define IWDG_LSI_FREQUENCY ((uint32_t)(0x8000))//晶振用的是32.768KHz石英晶振

    /*##-3- Configure the IWDG peripheral ######################################*/
    /*
       Set counter reload value to obtain 250ms IWDG TimeOut.
       IWDG counter clock Frequency = LsiFreq / 32
       Counter Reload Value = 250ms / IWDG counter clock period
                            = 0.25s / (32/LsiFreq)
                            = LsiFreq / (32 * 4)
                            = LsiFreq / 128
    */
    IwdgHandle.Instance = IWDG;

    IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
    //IwdgHandle.Init.Reload    = IWDG_LSI_FREQUENCY / 128;
    IwdgHandle.Init.Reload    = (uint16_t)((IWDG_LSI_FREQUENCY / 32)*(IwdgMs/1000.0));

    if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
    {
        /* Initialization Error */
        printf("独立看门狗初始化失败\r\n");
    }

    /*##-4- Start the IWDG #####################################################*/
    /*if (HAL_IWDG_Start(&IwdgHandle) != HAL_OK)//旧版库需要这个操作，新版的不需要
    {
        ;
    }*/

    /* Refresh IWDG: reload counter */
    if (HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK)
    {
        /* Refresh Error */
        printf("独立看门狗刷新失败\r\n");
    }
}

/*******************************************************************
*函数：void IWDG_PrmRefresh(void)
*功能：独立看门狗参数刷新（喂狗）
*输入：无
*输出：无
*返回：无
*特殊说明：无
*******************************************************************/
void IWDG_PrmRefresh(void)
{
    if (HAL_IWDG_Refresh(&IwdgHandle) != HAL_OK)
    {
        /* Refresh Error */
        printf("独立看门狗刷新失败\r\n");
    }
}

/************************ (C) COPYRIGHT NEWLAND_EDU *****END OF FILE****/
