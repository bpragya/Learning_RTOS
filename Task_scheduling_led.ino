//create two tasks to control the blinking rate of an LED
//one task should listen for input on the serial terminal. when you enter a number,
//the delay time on the blinking LED 
//should be updated to that time
//the other tasks causes the LED to blink at the proper delay time
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const int ledPin = 2;
static TaskHandle_t readSerialTask = NULL; //allows you to refer to a specific task
static TaskHandle_t toggleLEDTask = NULL;

static volatile int led_delay = 500; // ms, written by readSerial, read by toggleLED

void readSerial(void * parameter)
{
  while (1)
  {
    if (Serial.available() > 0) { //checks whether there is incoming data
      int value = Serial.parseInt();
      // parseInt returns 0 when there is no number (e.g. a stray '\n'), so ignore it
      if (value > 0) {
        led_delay = value;
        Serial.print("Value: ");
        Serial.println(led_delay);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(50)); //running -> blocked for 50 ms, so input feels responsive
  }
  
}

void toggleLED(void * parameter)
{
  while (1)
  {
    digitalWrite(ledPin, HIGH);
    vTaskDelay(pdMS_TO_TICKS(led_delay));
    digitalWrite(ledPin, LOW);
    vTaskDelay(pdMS_TO_TICKS(led_delay));
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //create your led pin
  pinMode(ledPin, OUTPUT);

  //create your tasks
  xTaskCreatePinnedToCore(
    readSerial,
    "read from serial terminal",
    2048, //1024 is too small once Serial.print is involved
    NULL,
    2, //made getting the number a higher priority task
    &readSerialTask, //?
    app_cpu
  );

  xTaskCreatePinnedToCore(
    toggleLED,
    "toggle LED",
    2048,
    NULL,
    1,
    &toggleLEDTask,
    app_cpu
  );

}

void loop() {
  // put your main code here, to run repeatedly:



}
