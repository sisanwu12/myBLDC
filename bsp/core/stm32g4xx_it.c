#include "FreeRTOS.h"
#include "bsp_board.h"
#include "stm32g4xx_it.h"
#include "task.h"

void xPortSysTickHandler(void);

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
}

void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();

  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
    xPortSysTickHandler();
  }
}
