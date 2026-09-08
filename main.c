#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "project_defs.h"

QueueHandle_t sensorQueue;
QueueHandle_t actuatorQueue;

int main()
{
    stdio_init_all();

    sensorQueue = xQueueCreate(10, sizeof(SensorData_t));
    actuatorQueue = xQueueCreate(10, sizeof(ActuatorCmd_t));

    TaskHandle_t sensorHandle;
    TaskHandle_t actuatorHandle;
    TaskHandle_t commHandle;

    xTaskCreate(sensorTask, "SensorTask", 1024, NULL, 1, &sensorHandle);
    xTaskCreate(actuatorTask, "ActuatorTask", 256, NULL, 3, &actuatorHandle);
    xTaskCreate(communicationTask, "CommTask", 4096, NULL, 2, &commHandle);

    // micro-ROS sul core 0, sensori/attuatori sul core 1
    vTaskCoreAffinitySet(sensorHandle, (1 << 1));
    vTaskCoreAffinitySet(actuatorHandle, (1 << 1));
    vTaskCoreAffinitySet(commHandle, (1 << 0));

    vTaskStartScheduler();

    while (1);
}