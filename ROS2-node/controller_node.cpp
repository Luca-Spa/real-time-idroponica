#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/bool.hpp"


class PicoControllerNode : public rclcpp::Node
{
public:
    PicoControllerNode() : Node("pico_controller_node")
    {
        // Publisher usando il tipo standard Bool
        pump_pub_ = this->create_publisher<std_msgs::msg::Bool>("/hydro/actuators/pump", 10);
        fan_pub_ = this->create_publisher<std_msgs::msg::Bool>("/hydro/actuators/fan", 10);

        // Subscriber per i sensori
        humidity_sub_ = this->create_subscription<std_msgs::msg::Float32>(
            "/sensors/humidity", 10,
            std::bind(&PicoControllerNode::humidity_callback, this, std::placeholders::_1));

        temperature_sub_ = this->create_subscription<std_msgs::msg::Float32>(
            "/sensors/temperature", 10,
            std::bind(&PicoControllerNode::temperature_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Pico Controller avviato. In attesa di dati...");
    }

private:
    bool pump_active_;
    bool fan_active_;

    void humidity_callback(const std_msgs::msg::Float32::SharedPtr msg) {
        bool desired_state = (msg->data < 30.0f);

        // Pubblica il comando solo se lo stato desiderato è diverso da quello attuale
        if (desired_state != pump_active_) {
            std_msgs::msg::Bool cmd;
            cmd.data = desired_state;
            pump_pub_->publish(cmd);
            
            pump_active_ = desired_state; // Aggiorna lo stato interno
            
            RCLCPP_INFO(this->get_logger(), "Umidità %.1f%% - POMPA impostata su: %s", 
                        msg->data, desired_state ? "ON" : "OFF");
        }
    }

    void temperature_callback(const std_msgs::msg::Float32::SharedPtr msg) {
        bool desired_state = (msg->data > 25.0f); 

        if (desired_state != fan_active_) {
            std_msgs::msg::Bool cmd;
            cmd.data = desired_state;
            fan_pub_->publish(cmd);
            
            fan_active_ = desired_state; 
            
            RCLCPP_INFO(this->get_logger(), "Temperatura %.1f°C - VENTOLA impostata su: %s", 
                        msg->data, desired_state ? "ON" : "OFF");
        }
    }

    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr pump_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr fan_pub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr humidity_sub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr temperature_sub_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PicoControllerNode>());
    rclcpp::shutdown();
    return 0;
}