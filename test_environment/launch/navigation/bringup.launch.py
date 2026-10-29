import os

from ament_index_python.packages import get_package_share_directory
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_xml.launch_description_sources import XMLLaunchDescriptionSource
from launch_ros.actions import Node
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PythonExpression
from launch import LaunchDescription

ARGUMENTS = [
    DeclareLaunchArgument('pose_estimator', default_value="'odometry'",
                          choices = ["'rgbd_odometry'", "'odometry'"],
                          description='Pose estimators for robot'),
    DeclareLaunchArgument('model', default_value='diff',
                          choices=['diff', 'rocker_bogie'],
                          description='Model to use for simulation'),
    DeclareLaunchArgument('mode', default_value='localization',
                          choices=['localization', 'mapping'],
                          description='work mode for navigation'),
    DeclareLaunchArgument('slam_type', default_value='toolbox',
                          choices = ['toolbox', 'rtabmap'],
                          description='slam solver for robot')
]

def evaluate_spawn(context, *args, **kwargs):
  current_package_name = 'test_environment'

  model = LaunchConfiguration('model').perform(context)
  pose_estimator = LaunchConfiguration('pose_estimator').perform(context)
  mode = LaunchConfiguration('mode').perform(context)
  slam_type = LaunchConfiguration('slam_type').perform(context)

  # nav2 param path
  nav2_param_path = os.path.join(get_package_share_directory(current_package_name), 'config', 'navigation', 'nav.yaml')

  # rgbd odom path
  rgbd_odom_config_path = os.path.join(get_package_share_directory(current_package_name), 'config', 'localization', 'rgdb_odometry.yaml')

  # ekf config's path
  ekf_config_dir = os.path.join(get_package_share_directory(current_package_name), 'config', 'localization')
  if pose_estimator == 'rgbd_odometry':
    ekf_config_name = 'ekf_vis_odom_based.yaml'
  else:
    ekf_config_name = 'ekf_el_classico.yaml'

  # launch's path
  rocker_bogie_cont_launch_path = os.path.join(get_package_share_directory(current_package_name), 'launch', 'controller', 'rocker_bogie.launch.py')
  diff_cont_launch_path = os.path.join(get_package_share_directory(current_package_name), 'launch', 'controller', 'diff.launch.py')

  # intertial, kinematic: odometry publisher (required)(as pose estimator)
  rocker_bogie_controller = IncludeLaunchDescription(PythonLaunchDescriptionSource(rocker_bogie_cont_launch_path),
                                                     condition = IfCondition(PythonExpression([repr(model), " == 'rocker_bogie'"])))
  diff_cont_controller = IncludeLaunchDescription(PythonLaunchDescriptionSource(diff_cont_launch_path),
                                                  condition = IfCondition(PythonExpression([repr(model), " == 'diff'"])))

  rgbd_odometry = Node(
    package="rtabmap_odom",
    executable="rgbd_odometry",
    name="rgbd_odometry",
    output="log",
    parameters=[rgbd_odom_config_path],
    remappings=[
        ("rgb/image", "/camera/image_raw"),
        ("rgb/camera_info", "/camera/camera_info"),
        ("depth/image", "/camera/depth/image_raw"),
        ("imu", "/imu"),
        ("odom", "/odom_rgbd")
    ],
    arguments=["--ros-args", "--log-level", "warn"],
    condition = IfCondition(PythonExpression([pose_estimator, " == 'rgbd_odometry'"]))
  )

  ekf = Node(
    package='robot_localization',
    executable='ekf_node',
    name='ekf_filter_node',
    output='screen',
    arguments=["--ros-args", "--log-level", "info"],
    parameters=[os.path.join(ekf_config_dir, ekf_config_name)]
  )

  nav2_container = Node(
    name='nav2_container',
    package='rclcpp_components',
    executable='component_container_isolated',
    parameters=[nav2_param_path, 
                {'autostart': 'true'}],
    arguments=['--ros-args', '--log-level', 'info'],
    output='screen'
  )

  mapping = IncludeLaunchDescription(
    PythonLaunchDescriptionSource(os.path.join(get_package_share_directory(current_package_name), 'launch', 'navigation', 'slam.launch.py')),
    condition=IfCondition(PythonExpression([repr(mode), " == 'mapping'"])),
    launch_arguments={'slam_algo': slam_type}.items()
  )

  loc = IncludeLaunchDescription(
    PythonLaunchDescriptionSource(os.path.join(get_package_share_directory(current_package_name), 'launch', 'navigation', 'localization.launch.py')),
    condition=IfCondition(PythonExpression([repr(mode), " == 'localization'"])),
    launch_arguments={'params_file': nav2_param_path}.items()
  )

  nav = IncludeLaunchDescription(
    PythonLaunchDescriptionSource(os.path.join(get_package_share_directory(current_package_name), 'launch', 'navigation', 'navigation.py')),
    launch_arguments={'params_file': nav2_param_path}.items()
  )

  return [rocker_bogie_controller,
          diff_cont_controller,
          rgbd_odometry,
          ekf,
          nav2_container,
          mapping,
          loc,
          nav]

def generate_launch_description():
  ld = LaunchDescription(ARGUMENTS)
  ld.add_action(OpaqueFunction(function=evaluate_spawn))
  return ld