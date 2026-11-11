# Learning RTOS

Small FreeRTOS exercises for the ESP32, written as Arduino sketches. Each `.ino` file is a standalone sketch that demonstrates one RTOS concept: creating tasks, passing data between them, and managing heap memory.

## Hardware and setup

- ESP32 dev board with an LED on **GPIO 2** (the onboard LED on most boards)
- Arduino IDE with the ESP32 board package installed
- Serial monitor at **115200 baud**, with line ending set to "Newline"

All sketches pin their tasks to a single core (`app_cpu`), which is core 1 on dual-core chips and core 0 on single-core chips.

## Sketches

| File | What it demonstrates |
| --- | --- |
| [blinkey.ino](blinkey.ino) | Two tasks blink the LED at two different rates and print "Task 1!" / "Task 2!" to Serial. |
| [Task_scheduling_led.ino](Task_scheduling_led.ino) | One task reads a number from the serial terminal and a second task blinks the LED using that number as the delay in ms. |
| [queue_task.ino](queue_task.ino) | A CLI task and an LED task talk over two queues. Typing `delay 200` sends 200 to the LED task through `queue1`. After 100 blinks the LED task sends "Blinked" back through `queue2`, and the CLI task prints it. |
| [memory_mmg_task.ino](memory_mmg_task.ino) | Passes heap-allocated strings between tasks through queues. The sender allocates with `pvPortMalloc` and queues the pointer. The receiver prints the string and frees it with `vPortFree`. |

## Usage

1. Open a sketch in the Arduino IDE.
2. Select your ESP32 board and port, then upload.
3. Open the serial monitor (115200 baud) and type input where the sketch expects it:
   - `Task_scheduling_led.ino`: type a number such as `250`.
   - `queue_task.ino`: type `delay 250`.
   - `memory_mmg_task.ino`: type a number to blink the LED at that delay. The text you type is also echoed back through the queue.

