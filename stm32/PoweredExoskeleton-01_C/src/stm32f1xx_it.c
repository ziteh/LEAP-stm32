/**
  ******************************************************************************
  * @file    stm32f1xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_it.h"
#include "stm32f10x.h"

/** @addtogroup IO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint8_t BlinkSpeed;
uint16_t nInst = 0;			// The number of instruction
uint8_t selMotor = 0xFF;	// The motor which be selected

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M Processor Exceptions Handlers                          */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_md.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles EXTI15_10_IRQHandler Handler.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if (EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) != RESET)
  {		
    if(BlinkSpeed == 1)
    {
      BlinkSpeed = 0;
    }
    else
    {
      BlinkSpeed ++;
    }
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }	
}

/**
  * @brief  This function handles USART2_IRQHandler Handler.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // NOT USART_FLAG_RXNE
	{
		uint16_t USART_ReceivData = 0xF0;
		USART_ReceivData = USART_ReceiveData(USART2);

		if(nInst == 0)
		{
//			USART_Send(USART2, "STM32:\n");

			if(USART_ReceivData == 0xE0)		// System stop
			{
				USART_Send(USART2, "[System]Stop.\n");
			}
			else if(USART_ReceivData == 0xE1)	// System reset
			{
				/* Initialization */
				// Functions & Setups
				RCC_Initialization();
				GPIO_Initialization();
				USART_Initialization();
				PWM_Initialization();
				NVIC_Initialization();

				// Reset all motor
				MotorCtrl(0, 0, 1, 0);	// Motor0: Disable, CCW, Speed:0
				MotorCtrl(1, 0, 0, 0);	// Motor1: Disable,  CW, Speed:0

				// Turn off LD2(User-LED)
				Pin_Clr(5); // #define LD2 (5)

				USART_Send(USART2, "[System]Reset.\n");
			}
			else if((USART_ReceivData & 0xE0) == 0x20)	// Instruction start
			{
				// Select motor
				while(selMotor != ((USART_ReceivData & 0x18) >> 3))
					selMotor = ((USART_ReceivData & 0x18) >> 3);

				// Set instruction number
				while(nInst != (USART_ReceivData & 0x07))
					nInst = (USART_ReceivData & 0x07);

				if(selMotor == 0x00)
					USART_Send(USART2, "[Control]Motor-0\n");
				else if(selMotor == 0x01)
					USART_Send(USART2, "[Control]Motor-1\n");
				else
					USART_Send(USART2, "[Error]No motors selected.\n");
			}
			else if(USART_ReceivData == 0xF0)
				/* Null */;
			else	// Unknown instruction
			{
				USART_Send(USART2, "[Error]Unknown instruction.\n");
			}
		}
		else	// nInst != 0
		{
			nInst--;

			// Set motor speed
			if(((USART_ReceivData & 0x80) >> 7) == 0x01) 		// 1xxx xxxx(b)
			{
				MotorCtrl(selMotor, 3, 3, (USART_ReceivData & 0x7F));
				USART_Send(USART2, " Set speed:");
				USART_Send(USART2, Number_TO_String(USART_ReceivData & 0x7F));
				USART_Send(USART2, "%\n");
			}
			else												// 0xxx xxxx(b)
			{
				/* Motor status */
				// Disable
				if(((USART_ReceivData & 0x0C) >> 2) == 0x00)	 // xxxx 00xx(b)
				{
					MotorCtrl(selMotor, 0, 3, 127);
					USART_Send(USART2, " Disable.\n");
				}
				// Enable
				else if(((USART_ReceivData & 0x0C) >> 2) == 0x01)// xxxx 01xx(b)
				{
					MotorCtrl(selMotor, 1, 3, 127);
					USART_Send(USART2, " Enable.\n");
				}
				// Toggle
				else if(((USART_ReceivData & 0x0C) >> 2) == 0x02)// xxxx 10xx(b)
				{
					MotorCtrl(selMotor, 2, 3, 127);
					USART_Send(USART2, " Toggle.\n");
				}
				// Keep
				else /* Null */;								 // xxxx 11xx(b)

				/* Motor direction */
				// CW
				if(((USART_ReceivData & 0x03)) == 0x00)		// xxxx xx00(b)
				{
					MotorCtrl(selMotor, 3, 0, 127);
					USART_Send(USART2, " Direction:CW.\n");
				}
				// CCW
				else if(((USART_ReceivData & 0x03)) == 0x01)// xxxx xx01(b)
				{
					MotorCtrl(selMotor, 3, 1, 127);
					USART_Send(USART2, " Direction:CCW.\n");
				}
				// Toggle
				else if(((USART_ReceivData & 0x03)) == 0x02)// xxxx xx10(b)
				{
					MotorCtrl(selMotor, 3, 2, 127);
					USART_Send(USART2, " Direction:Toggle.\n");
				}
				// Keep
				else /* Null */;							// xxxx xx11(b)
			}

			// End of instruction
			if(nInst == 0)
			{
				// Deselect motor
				while(selMotor != 0xFF) selMotor = 0xFF;
				USART_Send(USART2, "[Control]Done.\n");
			}
		}
		/* NO need to clears the USARTx's interrupt pending bits */
		/* USART_ClearITPendingBit(USART2,USART_IT_RXNE); */
	}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
