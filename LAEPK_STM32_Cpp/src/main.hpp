/**
 ******************************************************************************
 * @file    main.hpp
 * @author  Huang, Tzu-Fu
 *          National Formosa University
 *          Department of Electronic Engineering
 *          Intelligent Robot System Laboratory
 * @version V1.0.0
 * @date    25-October-2019
 * @brief   Header for main.cpp module
 ******************************************************************************
 * @attention
 *
 * None
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion */
#ifndef __MAIN_H
#define __MAIN_H

#include <string>
#include <sstream>
#include <cmath>
#include "GPIO_Functions.hpp"
#include "PWM_Functions.hpp"
#include "USART_Functions.hpp"
#include "ADC_Functions.hpp"
#include "EC90fl_Motor_Functions.hpp"
#include "joint.hpp"
#include "unit_test.hpp"

extern "C"
{
#include <stdio.h>
#include "stm32f10x.h"
}

#define RightJoint_PortPin_SpeedPWM ((GPIO_PortPinTypeDef)PA7)
#define LeftJoint_PortPin_SpeedPWM ((GPIO_PortPinTypeDef)PA7)

#define RightJoint_PortPin_FunctionState ((GPIO_PortPinTypeDef)D10)
#define LeftJoint_PortPin_FunctionState ((GPIO_PortPinTypeDef)D7)

/* GPIO mapping */
#define RightMotor_DirectionPin ((GPIO_PortPinTypeDef)D10)
#define RightMotor_FunctionStatePin ((GPIO_PortPinTypeDef)D9)

/* Default PWM value */
#define PWM_DefaultFrequncy ((uint16_t)5000)
#define PWM_DefaultDutyCycle ((uint8_t)15)

/* Default Joint ADC value */
#define Joint_DefaultFullExtensionADCValue ((uint16_t)1400)
#define Joint_DefaultFullFlexionADCValue ((uint16_t)2450)

/* Default FSR start threshold */
#define Joint_DefaultExtensionFSRStartThreshold ((uint16_t)215)
#define Joint_DefaultFlexionFSRStartThreshold ((uint16_t)180)

/* Default FSR stop threshold */
#define Joint_DefaultExtensionFSRStopThreshold ((uint16_t)500)
#define Joint_DefaultFlexionFSRStopThreshold ((uint16_t)500)

/**
 * @brief Initializing RCC.
 */
#define RCC_Initialization                            \
  {                                                   \
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 |    \
                               RCC_APB1Periph_TIM2 |  \
                               RCC_APB1Periph_TIM3,   \
                           ENABLE);                   \
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |     \
                               RCC_APB2Periph_GPIOB | \
                               RCC_APB2Periph_GPIOC | \
                               RCC_APB2Periph_ADC1,   \
                           ENABLE);                   \
  }

/**
 * @brief Initializing limit switch.
 * @remark RCC_APB2: GPIOA
 */
#define LimitSwitch_Initialization                              \
  {                                                             \
    GPIO limitSwitch;                                           \
    limitSwitch.PortPin = PA0;                                  \
    limitSwitch.Mode = GPIO_Mode_IPD;                           \
    limitSwitch.Init();                                         \
                                                                \
    NVIC_InitTypeDef NVIC_InitStructure;                        \
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;            \
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   \
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          \
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             \
    NVIC_Init(&NVIC_InitStructure);                             \
                                                                \
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); \
                                                                \
    EXTI_InitTypeDef EXTI_InitStructure;                        \
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;                  \
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;         \
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;     \
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                   \
    EXTI_Init(&EXTI_InitStructure);                             \
  }

/**
 * @brief Initializing USART.
 * @remark RCC_APB1: USART2
 *         RCC_APB2: GPIOA
 */
#define USART_Initialization                                                        \
  {                                                                                 \
    GPIO USART2_TX;                                                                 \
    USART2_TX.PortPin = PA2;                                                        \
    USART2_TX.Mode = GPIO_Mode_AF_PP;                                               \
    USART2_TX.Speed = GPIO_Speed_50MHz;                                             \
    USART2_TX.Init();                                                               \
                                                                                    \
    GPIO USART2_RX;                                                                 \
    USART2_RX.PortPin = PA3;                                                        \
    USART2_RX.Mode = GPIO_Mode_IN_FLOATING;                                         \
    USART2_RX.Init();                                                               \
                                                                                    \
    NVIC_InitTypeDef NVIC_InitStructure;                                            \
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                               \
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                       \
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                              \
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                 \
    NVIC_Init(&NVIC_InitStructure);                                                 \
                                                                                    \
    USART_InitTypeDef USART_InitStructure;                                          \
    USART_StructInit(&USART_InitStructure);                                         \
    USART_InitStructure.USART_BaudRate = 9600;                                      \
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     \
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          \
    USART_InitStructure.USART_Parity = USART_Parity_No;                             \
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; \
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 \
    USART_Init(USART2, &USART_InitStructure);                                       \
                                                                                    \
    /* Enable "Receive data register not empty" interrupt */                        \
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                  \
                                                                                    \
    /* Enable USART */                                                              \
    USART_Cmd(USART2, ENABLE);                                                      \
                                                                                    \
    /* Clear "Transmission Complete" flag, else the first bit of data will lose. */ \
    USART_ClearFlag(USART2, USART_FLAG_TC);                                         \
  }

/**
 * @brief Initializing timer.
 * @remark RCC_APB1: TIM2
 */
#define Timer_Initialization                                    \
  {                                                             \
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;              \
    TIM_TimeBaseStructure.TIM_Period = 2000;                    \
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;             \
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     \
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; \
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);             \
                                                                \
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);                       \
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                  \
    TIM_Cmd(TIM2, ENABLE);                                      \
                                                                \
    NVIC_InitTypeDef NVIC_InitStructure;                        \
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;             \
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   \
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          \
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             \
    NVIC_Init(&NVIC_InitStructure);                             \
  }

/**
 * @brief Initializing board.
 * @remark RCC_APB2: GPIOA, GPIOC
 */
#define Board_Initialization             \
  {                                      \
    GPIO Button;                         \
    Button.PortPin = User_Button;        \
    Button.Mode = GPIO_Mode_IN_FLOATING; \
    Button.Init();                       \
                                         \
    GPIO LED;                            \
    LED.PortPin = User_LED;              \
    LED.Mode = GPIO_Mode_Out_PP;         \
    LED.Speed = GPIO_Speed_2MHz;         \
    LED.Init();                          \
    LED.setValue(LOW);                   \
  }

typedef enum
{
  Right,
  Left
} JointTypeDef;

void MotionHandler(void);
void CommunicationDecoder(uint8_t Command);
void Delay_NonTimer(__IO uint32_t nTime);

void Joint_Initialization(Joint *joint, JointTypeDef jointType);

extern "C"
{
  void Delay_ms(__IO uint32_t nTime);
  void TimingDelay_Decrement(void);
}

#endif /* __MAIN_H */

/*********************************END OF FILE**********************************/
