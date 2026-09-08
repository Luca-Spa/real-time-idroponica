#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "project_defs.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/bool.h>
#include <rmw_microros/rmw_microros.h>

extern bool pico_serial_transport_open(struct uxrCustomTransport * transport);
extern bool pico_serial_transport_close(struct uxrCustomTransport * transport);
extern size_t pico_serial_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
extern size_t pico_serial_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

rcl_publisher_t temperature_pub;
rcl_publisher_t humidity_pub;
rcl_subscription_t pump_sub;
rcl_subscription_t fan_sub;

std_msgs__msg__Float32 temp_msg;
std_msgs__msg__Float32 hum_msg;
std_msgs__msg__Bool pump_cmd_msg;
std_msgs__msg__Bool fan_cmd_msg;

void pump_subscription_callback(const void * msgin) {
    const std_msgs__msg__Bool * msg = (const std_msgs__msg__Bool *)msgin;
    ActuatorCmd_t cmd;
    cmd.device = ACT_PUMP;
    cmd.state = msg->data;
    xQueueSendToBack(actuatorQueue, &cmd, pdMS_TO_TICKS(10));
}

void fan_subscription_callback(const void * msgin) {
    const std_msgs__msg__Bool * msg = (const std_msgs__msg__Bool *)msgin;
    ActuatorCmd_t cmd;
    cmd.device = ACT_FAN;
    cmd.state = msg->data;
    xQueueSendToBack(actuatorQueue, &cmd, pdMS_TO_TICKS(10));
}

void communicationTask(void *pvParameters) {
    rmw_uros_set_custom_transport(true, NULL, pico_serial_transport_open, pico_serial_transport_close, pico_serial_transport_write, pico_serial_transport_read);

    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;
    
    while(rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    rcl_node_t node;
    rclc_node_init_default(&node, "pico_node", "", &support);

    rclc_publisher_init_default(&temperature_pub, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "/hydro/sensors/temperature");
    rclc_publisher_init_default(&humidity_pub, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "/hydro/sensors/humidity");

    rclc_subscription_init_default(&pump_sub, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/hydro/control/pump");
    rclc_subscription_init_default(&fan_sub, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "/hydro/control/fan");

    rclc_executor_t executor;
    rclc_executor_init(&executor, &support.context, 2, &allocator);
    rclc_executor_add_subscription(&executor, &pump_sub, &pump_cmd_msg, &pump_subscription_callback, ON_NEW_DATA);
    rclc_executor_add_subscription(&executor, &fan_sub, &fan_cmd_msg, &fan_subscription_callback, ON_NEW_DATA);

    SensorData_t data;

    while (true) {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));

        if (xQueueReceive(sensorQueue, &data, 0) == pdPASS) {
            temp_msg.data = data.temperature;
            hum_msg.data = data.humidity;

            if (rcl_publish(&temperature_pub, &temp_msg, NULL) != RCL_RET_OK) {}
            if (rcl_publish(&humidity_pub, &hum_msg, NULL) != RCL_RET_OK) {}
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}