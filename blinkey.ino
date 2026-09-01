//Create two separate tasks that cause LED light to blink at two different speeds
//use one cpu
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = 2;

//created my task
void toggleLED_part2(void* params)
{
  while (1)
  {
    Serial.println("Task 2!");
    digitalWrite(led_pin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(5000));
    digitalWrite(led_pin, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void toggleLED(void * parameter)
{
  while (1)
  {
    Serial.println("Task 1!");
    digitalWrite(led_pin, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS); //non blocking
    digitalWrite(led_pin, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS); // 500 / 1 ms
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led_pin, OUTPUT);
  xTaskCreatePinnedToCore(toggleLED, "Toggle LED 1", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(toggleLED_part2, "Toggle LED 2", 1024, NULL, 1, NULL, app_cpu);
}

void loop() {
  // put your main code here, to run repeatedly:
}