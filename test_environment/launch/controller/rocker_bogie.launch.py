from launch_ros.actions import Node
from launch import LaunchDescription

def generate_launch_description():
  # ackermann kinematic solver (odometry topic publisher)
  rocker_bogie_drive_spawner = Node(
    package='controller_manager',
    executable='spawner',
    name='rocker_bogie_spawner',
    arguments=["rocker_bogie_cont"]
  )

  # joint state's publisher
  joint_broad_spawner = Node(
    package='controller_manager',
    executable='spawner',
    name='joint_broad_spawner',
    arguments=["joint_broad"]
  )

  ld = LaunchDescription()
  ld.add_action(rocker_bogie_drive_spawner)
  ld.add_action(joint_broad_spawner)
  return ld
