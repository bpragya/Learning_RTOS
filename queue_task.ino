#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = 2;
static const int blink_limit = 100;

// fixed-size message so it can be copied by value through queue2
struct Message
{
  char body[20];
};

QueueHandle_t queue1; // ints: new blink delay in ms (task1 -> task2)
QueueHandle_t queue2; // Message: text for the terminal (task2 -> task1)

// task1: CLI
// - prints new messages from queue2 to the serial terminal
// - echoes serial input back to the terminal
// - input "delay xxx" sends xxx to queue1
void task1(void * params)
{
  Message received;
  while (1)
  {
    // is there something in queue2? (non-blocking)
    if (xQueueReceive(queue2, &received, 0) == pdTRUE)
    {
      Serial.println(received.body);
    }

    // is there a line waiting on the serial terminal?
    if (Serial.available() > 0)
    {
      String input = Serial.readStringUntil('\n');
      input.trim(); // strip '\r' and spaces
      Serial.println(input); // echo

      if (input.startsWith("delay "))
      {
        int num = input.substring(6).toInt(); // parse from the line, not from Serial
        if (num > 0)
        {
          if (xQueueSend(queue1, &num, pdMS_TO_TICKS(5)) != pdTRUE)
          {
            Serial.println("queue is full");
          }
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10)); // yield so the idle task / watchdog can run
  }
}

// task2: blink the LED
// - monitors queue1 for a new delay value and updates the local variable
// - every 100 blinks, sends "Blinked" to queue2
void task2(void * params)
{
  int blink_delay = 500; // default ms
  int count = 0;
  Message msg;
  strcpy(msg.body, "Blinked");

  while (1)
  {
    // pick up a new delay if one arrived (non-blocking)
    int new_delay;
    if (xQueueReceive(queue1, &new_delay, 0) == pdTRUE)
    {
      blink_delay = new_delay;
    }

    digitalWrite(led_pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(blink_delay));
    digitalWrite(led_pin, LOW);
    vTaskDelay(pdMS_TO_TICKS(blink_delay));

    count++;
    if (count >= blink_limit)
    {
      count = 0;
      if (xQueueSend(queue2, &msg, 0) != pdTRUE)
      {
        Serial.println("queue2 full");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);

  queue1 = xQueueCreate(5, sizeof(int));
  queue2 = xQueueCreate(5, sizeof(Message));

  xTaskCreatePinnedToCore(task1, "CLI", 4096, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(task2, "Blink", 2048, NULL, 1, NULL, app_cpu);
}

void loop() {
}
