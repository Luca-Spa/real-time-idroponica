#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "project_defs.h"
#include "hardware/gpio.h"
#include <stdio.h>

#define FAN_PIN 15

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

TimerHandle_t pumpSafetyTimer;

// Perform initialisation
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

void vPumpSafetyCallback(TimerHandle_t xTimer) {
    pico_set_led(false); // Spento
    printf("EMERGENZA: Timer pompa scaduto! Spegnimento forzato.\n");
}

void actuatorTask(void *pvParameters) {
    ActuatorCmd_t cmd;

    if (pico_led_init() != PICO_OK) {
        printf("Errore inizializzazione LED/CYW43\n");
    }

    gpio_init(FAN_PIN);
    gpio_set_dir(FAN_PIN, GPIO_OUT);
    gpio_put(FAN_PIN, 0); // Spento

    pumpSafetyTimer = xTimerCreate("PumpTimer", pdMS_TO_TICKS(50000), pdFALSE, (void *)0, vPumpSafetyCallback);

    while (1) {
        if (xQueueReceive(actuatorQueue, &cmd, portMAX_DELAY) == pdPASS) {
            if (cmd.device == ACT_PUMP) {
                // Logica negata: se state è true, metto il pin a 0
                
                if (cmd.state == true) {
                    pico_set_led(true);
                    xTimerStart(pumpSafetyTimer, 0);
                } else {
                    pico_set_led(false);
                    xTimerStop(pumpSafetyTimer, 0);
                }
            } 
            else if (cmd.device == ACT_FAN) {
                gpio_put(FAN_PIN, cmd.state ? 1 : 0);
            }
        }
    }
}