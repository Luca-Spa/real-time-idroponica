#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "project_defs.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "dht.h"

#define HUMIDITY_ADC_PIN 26 // ADC0 sul Pico
#define DHT11_PIN 16

static dht_t dht;

float emulateTemperature(void){
    static float t = 20.0f;
    static float direction = 0.2f;
    t += direction;
    if (t >= 30.0f) direction = -0.2f;
    else if (t <= 20.0f) direction = 0.2f;
    return t;
}

float emulateHumidity(void){
    static float h = 40.0f;
    static float direction = -0.5f;
    h += direction;
    if (h >= 40.0f) direction = -0.5f;
    else if (h <= 20.0f) direction = 0.5f;
    return h;
}

/*
float readTemperature(dht_t *dht_instance){
    float temp, hum;
    dht_start_measurement(dht_instance);
    dht_result_t result = dht_finish_measurement_blocking(dht_instance, &hum, &temp);
    
    if (result == DHT_RESULT_OK) {
        return temp;
    } else {
        return -99.0f; 
    }
}

float readHumidity(void){
    adc_select_input(0);
    uint16_t adc_raw = adc_read();

    const uint16_t AIR_VAL = 3500;
    const uint16_t WATER_VAL = 1500;
    
    float soil_moisture = 100.0f - ((float)(adc_raw - WATER_VAL) / (AIR_VAL - WATER_VAL) * 100.0f);
    
    if (soil_moisture > 100.0f) soil_moisture = 100.0f;
    if (soil_moisture < 0.0f) soil_moisture = 0.0f;
    
    return soil_moisture;
}
*/

void sensorTask(void *pvParameters){
    SensorData_t data;

    adc_init();
    adc_gpio_init(HUMIDITY_ADC_PIN);
    dht_init(&dht, DHT11, pio0, DHT11_PIN, true);

    while (1){
        data.temperature = emulateTemperature();
        data.humidity = emulateHumidity();

        xQueueSend(sensorQueue, &data, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}