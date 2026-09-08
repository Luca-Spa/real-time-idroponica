#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "project_defs.h"
#include "hardware/gpio.h"
#include <stdio.h>

#define PUMP_RELAY_PIN 14
#define FAN_PIN 15

TimerHandle_t pumpSafetyTimer;

void vPumpSafetyCallback(TimerHandle_t xTimer) {
    gpio_put(PUMP_RELAY_PIN, 1); // 1 = Spento (assumendo relè logica negata)
    printf("EMERGENZA: Timer pompa scaduto! Spegnimento forzato.\n");
}

void actuatorTask(void *pvParameters) {
    ActuatorCmd_t cmd;

    gpio_init(PUMP_RELAY_PIN);
    gpio_set_dir(PUMP_RELAY_PIN, GPIO_OUT);
    gpio_put(PUMP_RELAY_PIN, 1); // Spento di default

    gpio_init(FAN_PIN);
    gpio_set_dir(FAN_PIN, GPIO_OUT);
    gpio_put(FAN_PIN, 0); // Spento di default (logica normale)

    pumpSafetyTimer = xTimerCreate("PumpTimer", pdMS_TO_TICKS(10000), pdFALSE, (void *)0, vPumpSafetyCallback);

    while (1) {
        if (xQueueReceive(actuatorQueue, &cmd, portMAX_DELAY) == pdPASS) {
            if (cmd.device == ACT_PUMP) {
                // Logica negata: se state è true, metto il pin a 0
                gpio_put(PUMP_RELAY_PIN, !cmd.state);
                
                if (cmd.state == true) {
                    xTimerStart(pumpSafetyTimer, 0);
                } else {
                    xTimerStop(pumpSafetyTimer, 0);
                }
            } 
            else if (cmd.device == ACT_FAN) {
                gpio_put(FAN_PIN, cmd.state ? 1 : 0);
            }
        }
    }
}