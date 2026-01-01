# STM32F407VET6 Bare Metal C++ Hello World

A bare metal C++ project for the STM32F407VET6 Black Board that blinks LEDs on PA6 and PA7.

## Hardware Requirements

- STM32F407VET6 Black Board (512KB Flash, 128KB RAM)
- ST-Link V2 or compatible programmer/debugger
- USB cable for power and programming

## Software Requirements

- ARM GCC Toolchain (`arm-none-eabi-gcc`)
- OpenOCD (for flashing and debugging)
- Make

### Installing ARM GCC Toolchain (macOS)

```bash
brew install gcc-arm-embedded
```

### Installing OpenOCD (macOS)

```bash
brew install openocd
```

## Project Structure

```
.
├── Makefile                      # Build configuration
├── STM32F407VETx_FLASH.ld       # Linker script
├── openocd.cfg                   # OpenOCD configuration
├── src/
│   ├── main.cpp                  # Main application (LED blink)
│   ├── stm32f407xx.h            # Device header file
│   ├── system_stm32f4xx.c       # System initialization
│   ├── system_stm32f4xx.h       # System header
│   └── startup_stm32f407xx.s    # Startup code and vector table
└── build/                        # Build output directory (created automatically)
```

## Building the Project

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

The program:
1. Initializes the system clock (using HSI - 16MHz internal oscillator)
2. Enables GPIOA clock
3. Configures PA6 and PA7 as outputs (push-pull, fast speed)
4. Alternately toggles PA6 and PA7 with delays (LED blink effect)

## LED Connections

On the STM32F407VET6 Black Board:
- LED1: PA6
- LED2: PA7

The LEDs will blink alternately when the program is running.

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

To use different GPIO pins for LEDs, modify the pin numbers in [src/main.cpp](src/main.cpp):

```cpp
/* Change these pin numbers as needed */
GPIOA->MODER &= ~(0x3U << (6 * 2));  /* Change 6 to your pin number */
GPIOA->MODER |= (0x1U << (6 * 2));
```

## License

This is a bare metal starter project - feel free to use and modify as needed.
