#include "FreeRTOS.h"
#include "bsp_board.h"
#include "task.h"

volatile const char *g_freertos_panic_reason = NULL;
volatile const char *g_freertos_panic_file = NULL;
volatile int g_freertos_panic_line = 0;
volatile const char *g_freertos_panic_task_name = NULL;

static void freertos_panic(const char *reason,
                           const char *file,
                           int line,
                           const char *task_name)
{
  g_freertos_panic_reason = reason;
  g_freertos_panic_file = file;
  g_freertos_panic_line = line;
  g_freertos_panic_task_name = task_name;

  __disable_irq();

  while (1)
  {
  }
}

void vAssertCalled(const char *file, int line)
{
  freertos_panic("assert", file, line, NULL);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  (void)xTask;

  freertos_panic("stack_overflow", NULL, 0, pcTaskName);
}

void vApplicationMallocFailedHook(void)
{
  freertos_panic("malloc_failed", NULL, 0, NULL);
}
