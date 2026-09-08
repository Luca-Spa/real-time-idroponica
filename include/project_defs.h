#ifndef PROJECT_DEFS_H
#define PROJECT_DEFS_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"

typedef enum {
    ACT_PUMP = 0,
    ACT_FAN  = 1
} ActuatorID_t;

typedef struct {
    float temperature;
    float humidity;
} SensorData_t;

typedef struct {
    ActuatorID_t device;
    bool state; 
} ActuatorCmd_t;

// Code condivise
extern QueueHandle_t sensorQueue;
extern QueueHandle_t actuatorQueue;

// Prototipi dei task
void sensorTask(void *pvParameters);
void actuatorTask(void *pvParameters);
void communicationTask(void *pvParameters);

#endif // PROJECT_DEFS_H