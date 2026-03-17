# myBLDC

一个面向 STM32G431RBT6 电机开发板的 BLDC 项目模板，当前已经完成 `APP / Module / BSP / HAL-CMSIS` 四层组织，并接入了 FreeRTOS v9 内核基础骨架。

## 1. 项目定位

本工程基于 STM32 HAL，使用 `CMake + Ninja` 构建，使用 `OpenOCD` 烧录，使用 VSCode `cortex-debug` 调试。

当前工程重点提供：

- 可直接交叉编译的 STM32G431RBT6 工程骨架
- 明确的 `APP / Module / BSP / HAL-CMSIS` 分层
- 已集成的 FreeRTOS 内核与 Cortex-M4F 端口
- 适合后续继续扩展为 BLDC 控制工程的任务化基础

当前工程仍然保持简化的首版状态：

- 板级外设驱动大多还是空实现占位
- `module_motor` 仅完成初始化接入，尚未进入控制循环
- 协议、故障等业务模块尚未补齐

## 2. 目录结构

```text
myBLDC/
├── app/
│   ├── app_main.c
│   ├── app_task.c
│   └── app_task.h
├── module/
│   └── motor/
│       ├── module_motor.c
│       └── module_motor.h
├── bsp/
│   ├── board/
│   ├── config/
│   ├── core/
│   ├── driver/
│   └── runtime/
├── Libraries/
│   ├── cmsis/
│   ├── freertos/
│   │   └── Source/
│   ├── hal/
│   ├── startup/
│   └── stm32g4/
├── project/
├── docs/
└── README.md
```

各层职责如下：

- `app/`
  负责系统启动编排、任务创建与调度启动。
- `module/`
  负责可复用功能模块。当前保留 `module/motor/` 作为 BLDC 业务入口。
- `bsp/`
  负责板级初始化、IRQ 桥接、HAL 配置、运行库适配和项目内部硬件接口。
- `Libraries/`
  放第三方底座，包括 HAL、CMSIS、startup，以及 FreeRTOS 内核源码。
- `project/`
  放构建层配置，包括 CMake、工具链文件、链接脚本和 SVD。

## 3. 启动链路

当前启动链路如下：

```text
startup_stm32g431xx.s
  -> Reset_Handler
  -> SystemInit()                              // Libraries/stm32g4/system_stm32g4xx.c
  -> __libc_init_array()
  -> main()                                    // app/app_main.c
  -> bsp_init()                                // bsp/board/bsp_init.c
     -> HAL_Init()
     -> bsp_clock_init()
     -> bsp_gpio_init()
     -> bsp_uart_init()
     -> bsp_pwm_init()
     -> bsp_adc_init()
     -> bsp_tim_init()
  -> app_init()                                // app/app_task.c
  -> app_start()
     -> 创建 motor / service / monitor 任务
     -> vTaskStartScheduler()
```

默认时钟策略保持不变：

- 使用 `HSI`
- 主频 `16 MHz`
- 不启用 `PLL`

## 4. FreeRTOS 接入方式

FreeRTOS 相关设计固定如下：

- 内核源码位于 `Libraries/freertos/Source`
- 编译接入文件固定为：
  - `list.c`
  - `queue.c`
  - `tasks.c`
  - `timers.c`
  - `event_groups.c`
  - `portable/GCC/ARM_CM4F/port.c`
  - `portable/MemMang/heap_4.c`
- 内核配置入口为 `bsp/config/FreeRTOSConfig.h`
- 内存策略使用 `heap_4`
- HAL 与 FreeRTOS 共享 `SysTick`

`SysTick_Handler()` 位于 `bsp/core/stm32g4xx_it.c`，其行为为：

```c
HAL_IncTick();
HAL_SYSTICK_IRQHandler();

if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
{
  xPortSysTickHandler();
}
```

这样可以同时满足两件事：

- 调度器启动前，`HAL_Delay()` 等 HAL 时基仍可正常使用
- 调度器启动后，FreeRTOS 正常接管 tick 调度

注意：`bsp/config/stm32g4xx_hal_conf.h` 中的 `USE_RTOS` 仍保持 `0U`，这是 STM32G4 当前 HAL 版本的要求，不影响 FreeRTOS 内核实际使用。

## 5. 当前 APP 接口与任务骨架

顶层 APP 接口当前固定为：

```c
void app_init(void);
void app_start(void);
```

当前首版任务骨架如下：

- `motor` 任务
  - 优先级：`tskIDLE_PRIORITY + 3`
  - 栈深：`384`
  - 周期：`1 ms`
  - 作用：保留电机主任务位置，当前仅维护心跳
- `service` 任务
  - 优先级：`tskIDLE_PRIORITY + 2`
  - 栈深：`256`
  - 周期：`10 ms`
  - 作用：为未来协议、故障、系统服务预留执行位
- `monitor` 任务
  - 优先级：`tskIDLE_PRIORITY + 1`
  - 栈深：`256`
  - 周期：`1000 ms`
  - 作用：维护任务心跳、任务数量、栈高水位与堆余量

为了调试方便，`app/app_task.c` 中保留了多组 `volatile` 诊断变量，可直接在调试器中观察任务运行状态。

## 6. 构建设计

真正的编译和链接逻辑位于 `project/CMakeLists.txt`。

当前 CMake 会：

- 递归收集 `app/`、`module/`、`bsp/` 下的源码
- 编译 `Libraries/hal` 下的 HAL 源码
- 编译 `Libraries/freertos/Source` 中选定的 FreeRTOS 内核文件
- 编译系统文件和启动文件
- 使用 `project/STM32G431RBTX_FLASH.ld` 进行链接
- 构建后生成 `elf / hex / bin / map`

默认产物名称保持不变：

- `build/template.elf`
- `build/template.hex`
- `build/template.bin`
- `build/template.map`

## 7. 快速开始

### 7.1 配置工程

在仓库根目录执行：

```bash
cmake \
  -DCMAKE_TOOLCHAIN_FILE="${PWD}/project/arm-gnu-toolchain.cmake" \
  -DCMAKE_SYSTEM_NAME=Generic \
  -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
  -GNinja \
  -S project \
  -B build
```

### 7.2 编译工程

```bash
cmake --build build
```

### 7.3 烧录工程

```bash
openocd \
  -f interface/cmsis-dap.cfg \
  -f target/stm32g4x.cfg \
  -c "program build/template.elf verify reset exit"
```

### 7.4 调试工程

VSCode 调试配置 `Debug with OpenOCD` 使用：

- 可执行文件：`build/template.elf`
- SVD：`project/STM32G431.svd`
- OpenOCD 配置：`interface/cmsis-dap.cfg + target/stm32g4x.cfg`

## 8. 开发建议

- 应用调度与任务创建优先放在 `app/`
- BLDC 业务逻辑优先放在 `module/`
- 硬件访问统一收敛到 `bsp/driver/`
- HAL 配置只改 `bsp/config/stm32g4xx_hal_conf.h`
- FreeRTOS 配置只改 `bsp/config/FreeRTOSConfig.h`
- 不要把自定义业务代码写进 `Libraries/`

当前最自然的下一步扩展方向是：

- 在 `module/motor/` 中接入真实的控制循环
- 在 `bsp/driver/` 中补齐 PWM、ADC、电流采样与同步定时器接口
- 根据业务需要新增 `protocol`、`fault` 等模块与对应任务/通知机制
