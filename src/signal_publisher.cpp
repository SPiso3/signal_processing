#include <chrono>
#include <cmath>
#include <memory>
#include <random>
#include <std_msgs/msg/detail/float64__struct.hpp>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

class SignalPublisher : public rclcpp::Node {
public:
  SignalPublisher()
      : Node("signal_publisher") // node name
  {
    // parameters
    sine_amplitude_ = this->declare_parameter<double>("amplitude", 1.0);
    sine_frequency_ = this->declare_parameter<double>("frequency", 1.0);
    noise_ratio_ = this->declare_parameter<double>("noise_ratio", 0.1);
    dt_ = this->declare_parameter<double>("dt", 0.01);

    // ros
    publisher_ =
        this->create_publisher<std_msgs::msg::Float64>("sine_signal/raw", 10);

    timer_ = this->create_wall_timer(
        std::chrono::duration<double>(dt_),
        std::bind(&SignalPublisher::timer_callback, this));

    // noise
    rng_.seed(std::random_device{}()); // seed once from hardware entropy
    noise_dist_ = std::normal_distribution<double>(0.0, 1.0);

    std::cout << "Sine SignalPublisher created! \n";
  }

private:
  // ROS handles
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;

  // signal settings
  double sine_amplitude_;
  double sine_frequency_;
  double noise_ratio_;
  double dt_;

  int k_;

  // noise
  std::mt19937 rng_;
  std::normal_distribution<double> noise_dist_;

  void timer_callback() {
    // publish message
    double t = k_ * dt_;
    k_++;
    double val = sine_amplitude_ * (std::sin(2 * M_PI * sine_frequency_ * t) +
                                    noise_dist_(rng_) * noise_ratio_);
    std_msgs::msg::Float64 msg;
    msg.data = val;
    publisher_->publish(msg);
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv); // start ROS
  rclcpp::spin(
      std::make_shared<SignalPublisher>()); // run callbacks until Ctrl+C
  rclcpp::shutdown();
  return 0;
}
