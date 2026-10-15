#include <memory>
#include <functional>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/bool.hpp"

bool pump_active = false;
bool fan_active = false;

rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr pump_pub;
rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr fan_pub;

void humidity_callback(const std_msgs::msg::Float32::SharedPtr msg){
    bool desired_state = (msg->data < 30.0f);

    if (desired_state != pump_active) {
        std_msgs::msg::Bool cmd;
        cmd.data = desired_state;

        pump_pub->publish(cmd);
        pump_active = desired_state;

        const char* stato;
        if (desired_state) {
            stato = "ON";
        } else {
            stato = "OFF";
        }

        RCLCPP_INFO(
            rclcpp::get_logger("pico_controller_node"),
            "Umidità %.1f%% - pompa impostata su: %s",
            msg->data, stato);
    }
}

void temperature_callback(const std_msgs::msg::Float32::SharedPtr msg){
    bool desired_state = (msg->data > 25.0f);

    if (desired_state != fan_active) {
        std_msgs::msg::Bool cmd;
        cmd.data = desired_state;

        fan_pub->publish(cmd);
        fan_active = desired_state;

        const char* stato;
        if (desired_state) {
            stato = "ON";
        } else {
            stato = "OFF";
        }

        RCLCPP_INFO(
            rclcpp::get_logger("pico_controller_node"),
            "Temperatura %.1f°C - ventola impostata su: %s",
            msg->data, stato);
    }
}

int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);

    auto node = std::make_shared<rclcpp::Node>(
        "pico_controller_node");

    pump_pub =
        node->create_publisher<std_msgs::msg::Bool>(
            "/hydro/control/pump", 10);

    fan_pub =
        node->create_publisher<std_msgs::msg::Bool>(
            "/hydro/control/fan", 10);

    auto humidity_sub =
        node->create_subscription<std_msgs::msg::Float32>(
            "/hydro/sensors/humidity", 10,
            humidity_callback);

    auto temperature_sub =
        node->create_subscription<std_msgs::msg::Float32>(
            "/hydro/sensors/temperature", 10,
            temperature_callback);

    RCLCPP_INFO(
        node->get_logger(),
        "Attesa dati...");

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}