#include "app_task.h"

#include "FreeRTOS.h"
#include "bsp_board.h"
#include "display/module_display.h"
#include "motor/module_motor.h"
#include "portable.h"
#include "task.h"

#include <stdbool.h>

#define APP_MOTOR_TASK_PRIORITY   ( tskIDLE_PRIORITY + 3U )
#define APP_SERVICE_TASK_PRIORITY ( tskIDLE_PRIORITY + 2U )
#define APP_MONITOR_TASK_PRIORITY ( tskIDLE_PRIORITY + 1U )

#define APP_MOTOR_TASK_STACK_DEPTH   384U
#define APP_SERVICE_TASK_STACK_DEPTH 256U
#define APP_MONITOR_TASK_STACK_DEPTH 256U

volatile uint32_t g_app_motor_task_heartbeat = 0U;
volatile uint32_t g_app_service_task_heartbeat = 0U;
volatile uint32_t g_app_monitor_task_heartbeat = 0U;
volatile UBaseType_t g_app_rtos_task_count = 0U;
volatile UBaseType_t g_app_motor_task_stack_high_water = 0U;
volatile UBaseType_t g_app_service_task_stack_high_water = 0U;
volatile UBaseType_t g_app_monitor_task_stack_high_water = 0U;
volatile size_t g_app_rtos_free_heap_bytes = 0U;
volatile size_t g_app_rtos_min_ever_free_heap_bytes = 0U;
volatile bool g_app_oled_ready = false;

static TaskHandle_t motor_task_handle = NULL;
static TaskHandle_t service_task_handle = NULL;
static TaskHandle_t monitor_task_handle = NULL;

static void app_motor_task(void *argument);
static void app_service_task(void *argument);
static void app_monitor_task(void *argument);
static void app_create_task(TaskFunction_t task_code,
                            const char *name,
                            uint16_t stack_depth,
                            UBaseType_t priority,
                            TaskHandle_t *task_handle);

void app_init(void)
{
  module_motor_init();

  g_app_motor_task_heartbeat = 0U;
  g_app_service_task_heartbeat = 0U;
  g_app_monitor_task_heartbeat = 0U;
  g_app_rtos_task_count = 0U;
  g_app_motor_task_stack_high_water = 0U;
  g_app_service_task_stack_high_water = 0U;
  g_app_monitor_task_stack_high_water = 0U;
  g_app_rtos_free_heap_bytes = 0U;
  g_app_rtos_min_ever_free_heap_bytes = 0U;
  g_app_oled_ready = false;
  motor_task_handle = NULL;
  service_task_handle = NULL;
  monitor_task_handle = NULL;

  if (module_display_init())
  {
    g_app_oled_ready = module_display_show_demo();
  }
}

void app_start(void)
{
  app_create_task(app_motor_task,
                  "motor",
                  APP_MOTOR_TASK_STACK_DEPTH,
                  APP_MOTOR_TASK_PRIORITY,
                  &motor_task_handle);
  app_create_task(app_service_task,
                  "service",
                  APP_SERVICE_TASK_STACK_DEPTH,
                  APP_SERVICE_TASK_PRIORITY,
                  &service_task_handle);
  app_create_task(app_monitor_task,
                  "monitor",
                  APP_MONITOR_TASK_STACK_DEPTH,
                  APP_MONITOR_TASK_PRIORITY,
                  &monitor_task_handle);

  vTaskStartScheduler();

  Error_Handler();
}

static void app_motor_task(void *argument)
{
  TickType_t last_wake_time = xTaskGetTickCount();

  (void)argument;

  for (;;)
  {
    g_app_motor_task_heartbeat++;
    vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1U));
  }
}

static void app_service_task(void *argument)
{
  TickType_t last_wake_time = xTaskGetTickCount();

  (void)argument;

  for (;;)
  {
    g_app_service_task_heartbeat++;
    vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(10U));
  }
}

static void app_monitor_task(void *argument)
{
  TickType_t last_wake_time = xTaskGetTickCount();

  (void)argument;

  for (;;)
  {
    g_app_monitor_task_heartbeat++;
    g_app_rtos_task_count = uxTaskGetNumberOfTasks();
    g_app_motor_task_stack_high_water =
        (motor_task_handle != NULL) ? uxTaskGetStackHighWaterMark(motor_task_handle) : 0U;
    g_app_service_task_stack_high_water =
        (service_task_handle != NULL) ? uxTaskGetStackHighWaterMark(service_task_handle) : 0U;
    g_app_monitor_task_stack_high_water =
        (monitor_task_handle != NULL) ? uxTaskGetStackHighWaterMark(monitor_task_handle) : 0U;
    g_app_rtos_free_heap_bytes = xPortGetFreeHeapSize();
    g_app_rtos_min_ever_free_heap_bytes = xPortGetMinimumEverFreeHeapSize();

    vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1000U));
  }
}

static void app_create_task(TaskFunction_t task_code,
                            const char *name,
                            uint16_t stack_depth,
                            UBaseType_t priority,
                            TaskHandle_t *task_handle)
{
  if (xTaskCreate(task_code, name, stack_depth, NULL, priority, task_handle) != pdPASS)
  {
    Error_Handler();
  }
}
