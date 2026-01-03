# STM32F407VET6 Slider Controller with FreeRTOS

A bare metal C++ project for the STM32F407VET6 Black Board featuring FreeRTOS task scheduling, LED control, PWM output, and quadrature encoder decoding.

## Hardware Requirements

- STM32F407VET6 Black Board (512KB Flash, 128KB RAM)
- ST-Link V2 or compatible programmer/debugger
- USB cable for power and programming

## Software Requirements

- ARM GCC Toolchain (`arm-none-eabi-gcc`)
- OpenOCD (for flashing and debugging)
- Make
- Git (for cloning FreeRTOS kernel)

### Installing ARM GCC Toolchain (macOS)

```bash
brew install gcc-arm-embedded
```

### Installing OpenOCD (macOS)

```bash
brew install openocd
```

## FreeRTOS Setup

This project uses FreeRTOS v10.6.2 for real-time task scheduling. The FreeRTOS kernel is not included in the repository and must be cloned separately:

```bash
git clone --depth 1 --branch V10.6.2 https://github.com/FreeRTOS/FreeRTOS-Kernel.git FreeRTOS
```

For detailed FreeRTOS configuration and usage information, see [docs/FREERTOS.md](docs/FREERTOS.md).

## Project Structure

```
.
├── Makefile                      # Build configuration
├── STM32F407VETx_FLASH.ld       # Linker script
├── openocd.cfg                   # OpenOCD configuration
├── FreeRTOS/                     # FreeRTOS kernel (not in repo - clone separately)
├── docs/
│   ├── FREERTOS.md              # FreeRTOS integration documentation
│   └── QUADRATURE_DECODER.md    # Quadrature encoder documentation
├── inc/
│   ├── FreeRTOSConfig.h         # FreeRTOS configuration
│   ├── led.h                     # LED control header
│   ├── pwm_driver.h             # PWM driver header
│   ├── quadrature_decoder.h     # Encoder decoder header
│   ├── stm32f407xx.h            # Device header file
│   ├── system_stm32f4xx.h       # System header
│   └── timer_common.h           # Common timer definitions
├── src/
│   ├── main.cpp                  # Main application with FreeRTOS tasks
│   ├── led.cpp                   # LED control implementation
│   ├── pwm_driver.cpp           # PWM driver implementation
│   ├── quadrature_decoder.cpp   # Encoder decoder implementation
│   ├── system_stm32f4xx.c       # System initialization
│   └── startup_stm32f407xx.s    # Startup code and vector table
└── build/                        # Build output directory (created automatically)
```

## Building the Project

First, ensure you have cloned the FreeRTOS kernel (see FreeRTOS Setup above).

To build the project:

```bash
make
```

This will create:
- `build/stm32f407_hello.elf` - ELF executable
- `build/stm32f407_hello.hex` - Intel HEX format
- `build/stm32f407_hello.bin` - Binary format
- `build/stm32f407_hello.map` - Memory map

To clean build artifacts:

```bash
make clean
```

## Flashing to the Board

Connect your ST-Link V2 to the board and run:

```bash
make flash
```

This uses OpenOCD to program the microcontroller.

Alternatively, you can flash manually:

```bash
openocd -f openocd.cfg -c "program build/stm32f407_hello.elf verify reset exit"
```

## How It Works

The program uses FreeRTOS for task scheduling with the following features:

1. **System Initialization**: Sets up system clock and hardware peripherals
2. **FreeRTOS Tasks**: Creates and manages multiple concurrent tasks
3. **LED Control**: Implements LED blinking using FreeRTOS task delays
4. **PWM Generation**: Provides PWM output for motor control or other applications
5. **Quadrature Decoder**: Reads position and direction from rotary encoders

For detailed information about each component, see the documentation files in the `docs/` directory.

## LED Connections

On the STM32F407VET6 Black Board:
- LED1: PA6
- LED2: PA7

The LEDs blink using FreeRTOS tasks with independent timing periods.

## Debugging

To debug with OpenOCD and GDB:

1. Start OpenOCD in one terminal:
```bash
openocd -f openocd.cfg
```

2. In another terminal, start GDB:
```bash
arm-none-eabi-gdb build/stm32f407_hello.elf
```

3. Connect to OpenOCD:
```
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

## Memory Configuration

- Flash: 512KB (0x08000000 - 0x0807FFFF)
- RAM: 128KB (0x20000000 - 0x2001FFFF)
- CCM RAM: 64KB (0x10000000 - 0x1000FFFF)

## Customization

To modify LED behavior, PWM settings, or encoder configuration, see the respective source files:
- LED control: [src/led.cpp](src/led.cpp) and [inc/led.h](inc/led.h)
- PWM driver: [src/pwm_driver.cpp](src/pwm_driver.cpp) and [inc/pwm_driver.h](inc/pwm_driver.h)
- Quadrature decoder: [src/quadrature_decoder.cpp](src/quadrature_decoder.cpp) and [inc/quadrature_decoder.h](inc/quadrature_decoder.h)
- FreeRTOS configuration: [inc/FreeRTOSConfig.h](inc/FreeRTOSConfig.h)

## License

This is a starter project for embedded development with FreeRTOS - feel free to use and modify as needed.
