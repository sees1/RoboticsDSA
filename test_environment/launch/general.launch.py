import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch.launch_description_sources import PythonLaunchDescriptionSource

from launch_ros.actions import Node, SetParameter

ARGUMENTS = [
    DeclareLaunchArgument('world', default_value='level1',
                          description='World model name'),
    DeclareLaunchArgument('spawn_robot', default_value='True',
                          choices=['True', 'False'],
                          description='Spawn the eureka robot model.'),
    DeclareLaunchArgument('internal_nav', default_value='True',
                          choices=['True', 'False'],
                          description='Internal navigation'),
    DeclareLaunchArgument('pose_estimator', default_value="'odometry'",
                          choices = ["'eagleye'", "'rgbd_odometry'", "'odometry'"],
                          description='Pose estimators for eureka robot'),
    DeclareLaunchArgument('model', default_value='diff',
                          choices=['diff', 'rocker_bogie'],
                          description='Model to use for simulation'),
    DeclareLaunchArgument('slam_type', default_value='toolbox',
                          choices = ['toolbox', 'rtabmap'],
                          description='slam solver for robot')
]


def generate_launch_description():

  package_name = 'test_environment'

  world_name = LaunchConfiguration('world')
  model_type = LaunchConfiguration('model')
  spawn_robot = LaunchConfiguration('spawn_robot')
  internal_nav = LaunchConfiguration('internal_nav')
  pose_estimator = LaunchConfiguration('pose_estimator')
  slam_type = LaunchConfiguration('slam_type')

  # launch's path
  world_launch_path = os.path.join(get_package_share_directory(package_name), 'launch', 'world', 'world_create.launch.py')
  localization_launch_path = os.path.join(get_package_share_directory(package_name), 'launch', 'navigation', 'localization.launch.py')
  navigation_launch_path = os.path.join(get_package_share_directory(package_name), 'launch', 'navigation', 'navigation.launch.py')

  # rviz config
  rviz2_config = PathJoinSubstitution([get_package_share_directory(package_name), 'rviz', 'common.rviz'])

  #--------------------------------------------------------------------------------------------------------------------------------------------------------

  # world initialization
  world = IncludeLaunchDescription(PythonLaunchDescriptionSource(world_launch_path),
                                   launch_arguments={'world': world_name,
                                                     'model': model_type,
                                                     'spawn_robot': spawn_robot}.items()
  )

  # dynamic localization initialization
  localization = IncludeLaunchDescription(PythonLaunchDescriptionSource(localization_launch_path),
                                          launch_arguments={'pose_estimator': pose_estimator,
                                                            'model': model_type}.items(),
                                          condition=IfCondition(internal_nav)
  )

  # mapping initialization
  navigation = IncludeLaunchDescription(PythonLaunchDescriptionSource(navigation_launch_path),
                                        launch_arguments={'slam_type': slam_type}.items(),
                                        condition=IfCondition(internal_nav)
  )

  # rviz initialization
  rviz = Node(package='rviz2',
             executable='rviz2',
             name='rviz2',
             arguments=['-d', rviz2_config],
             parameters=[],
             remappings=[
                ('/tf', 'tf'),
                ('/tf_static', 'tf_static')
             ],
             output='screen'
  )

  use_sim_time_param = SetParameter(name='use_sim_time', value=True)

  ld = LaunchDescription(ARGUMENTS)
  ld.add_action(use_sim_time_param)
  ld.add_action(world)
  ld.add_action(localization)
  ld.add_action(navigation)
  ld.add_action(rviz)
  return ld