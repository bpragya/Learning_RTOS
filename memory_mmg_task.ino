#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

QueueHandle_t queue_delay;
QueueHandle_t queue_msg;
static const int led_pin = 2;
int count = 0;
int input = 0;


void readSerial(void * parameters)
{
  while(1)
  {
    if (Serial.available() > 0)
    {
      String val = Serial.readStringUntil('\n');
      input = val.toInt(); //nothing to parse bc not in the buffer

      //store str in heap memory

      char * buffer = (char*)pvPortMalloc((val.length() + 1) * sizeof(char));

      //copy into buffer-stop here
      if (buffer != NULL)
      {
        strncpy(buffer, val.c_str(), val.length() + 1);
        //notify task B of new message - pointer the POINTER in the queue
        // FIX: free the buffer if the queue is full, otherwise it leaks
        if (xQueueSend(queue_msg, &buffer, 0) != pdTRUE)
        {
          vPortFree(buffer);
        }
      }

      //led light stuff
      if (input > 0) {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(pdMS_TO_TICKS(input));
        Serial.println("LOW");
        digitalWrite(led_pin,LOW);
        vTaskDelay(pdMS_TO_TICKS(input));
        count+=1;
      }

      if (count == 100)
      {
        count = 0; // FIX: reset so it only sends once
        // FIX: send a heap-allocated char* (queue holds pointers), not the String
        char * blinked = (char*)pvPortMalloc(strlen("blinked") + 1);
        if (blinked != NULL)
        {
          strcpy(blinked, "blinked");
          // FIX: free if the queue is full, otherwise it leaks
          if (xQueueSend(queue_delay, &blinked, 0) != pdTRUE)
          {
            vPortFree(blinked);
          }
        }
      }

    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void printMsg(void * parameters)
{
  char* recieved; //stores the output
  while (1)
  {
    // FIX: removed Serial.read() here, it stole characters from readSerial

    //is there anything in the queue? if yes, then print it
    // FIX: short timeout instead of portMAX_DELAY so the second queue gets checked
    if (xQueueReceive(queue_msg, (void*)&recieved, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      Serial.println(recieved);
      vPortFree(recieved); //free the memory
      recieved = NULL; //still holds the old address
      vTaskDelay(pdMS_TO_TICKS(10)); //task gives up the cpu
    }

    // FIX: receive a char* and free that, not &blinked
    if (xQueueReceive(queue_delay, (void*)&recieved, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      Serial.println(recieved);
      vPortFree(recieved);
      recieved = NULL;
      vTaskDelay(pdMS_TO_TICKS(10));
    }

  }
}

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(led_pin, OUTPUT);
  queue_delay = xQueueCreate(5, sizeof(char*)); //how do different lengths affect the code?
  queue_msg = xQueueCreate(5, sizeof(char*)); // FIX: was blinked.length() == 0 slots
  // FIX: stack 1024 -> 4096 (String + Serial.println overflow 1024)
  xTaskCreatePinnedToCore(readSerial, "read serial", 4096, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(printMsg, "print message", 4096, NULL, 1, NULL, app_cpu);
}

void loop() {
  // put your main code here, to run repeatedly:

}
