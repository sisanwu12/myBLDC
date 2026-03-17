#include "app_task.h"
#include "bsp_board.h"
#include "bsp_init.h"

int main(void)
{
  bsp_init();
  app_init();
  app_start();

  Error_Handler();
}
