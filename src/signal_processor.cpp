#include <functional>
#include <iostream>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

using std::placeholders::_1;

class SignalProcessor : public rclcpp::Node {
public:
  SignalProcessor() : Node("signal_processor") {
    // dt comes from the shared "/**" section of params.yaml
    dt_ = this->declare_parameter<double>("dt", 0.01);

    // Publishers first, so they exist before the first message arrives
    derivative_pub_ = this->create_publisher<std_msgs::msg::Float64>(
        "sine_signal/derivative", 10);
    integral_pub_ = this->create_publisher<std_msgs::msg::Float64>(
        "sine_signal/integral", 10);

    // Subscription
    subscription_ = this->create_subscription<std_msgs::msg::Float64>(
        "sine_signal/raw", 10,
        std::bind(&SignalProcessor::signal_callback, this, _1));
    std::cout << "Sine SignalProcessor created!\n";
  }

private:
  void signal_callback(const std_msgs::msg::Float64 &msg) {
    const double x = msg.data;

    // first sample
    if (!has_prev_) {
      prev_x_ = x;
      has_prev_ = true;
      return;
    }

    // Derivative: backward difference
    std_msgs::msg::Float64 derivative;
    derivative.data = (x - prev_x_) / dt_;
    derivative_pub_->publish(derivative);

    // Integral: trapezoidal rule, accumulated over time
    integral_ += 0.5 * (x + prev_x_) * dt_;
    std_msgs::msg::Float64 integral;
    integral.data = integral_;
    integral_pub_->publish(integral);
    // std::cout << "integral: " << integral_ << " derivative: " <<
    // derivative.data
    //           << "\n";

    prev_x_ = x;
  }

  // ROS handles
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscription_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr derivative_pub_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr integral_pub_;

  // Parameters
  double dt_;

  // State carried between callbacks
  double prev_x_{0.0};
  double integral_{0.0};
  bool has_prev_{false};
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SignalProcessor>());
  rclcpp::shutdown();
  return 0;
}
