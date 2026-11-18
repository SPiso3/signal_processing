import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    # Find this package's installed share/ folder, wherever the workspace is
    pkg_share = get_package_share_directory('signal_processing')
    params_file = os.path.join(pkg_share, 'config', 'params.yaml')
    layout_file = os.path.join(pkg_share, 'config', 'plotjuggler_layout.xml')

    return LaunchDescription([
        # Launch argument: "ros2 launch ... plot:=false" skips PlotJuggler
        DeclareLaunchArgument(
            'plot', default_value='true',
            description='Start PlotJuggler with the saved layout'),

        Node(
            package='signal_processing',
            executable='signal_publisher',
            name='signal_publisher',      # must match the key in params.yaml
            parameters=[params_file],
            output='screen',
        ),
        Node(
            package='signal_processing',
            executable='signal_processor',
            name='signal_processor',
            parameters=[params_file],
            output='screen',
        ),
        Node(
            package='plotjuggler',
            executable='plotjuggler',
            arguments=['-n', '-l', layout_file],
            condition=IfCondition(LaunchConfiguration('plot')),
            output='screen',
        ),
    ])
