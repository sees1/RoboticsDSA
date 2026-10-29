import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import LoadComposableNodes
from launch_ros.actions import Node
from launch_ros.descriptions import ComposableNode, ParameterFile
from nav2_common.launch import RewrittenYaml

current_package_share = get_package_share_directory('test_environment')

ARGUMENTS = [
  DeclareLaunchArgument('params_file', default_value=os.path.join(current_package_share, 'config', 'nav.yaml'),
    description='Full path to the ROS2 parameters file to use for all launched nodes'),
  DeclareLaunchArgument('costmap_file', default_value=os.path.join(current_package_share, 'maps', 'level1.yaml'),
    description='Full path to the ROS2 costmap .yaml file'),
]

def evaluate_spawn(context, *args, **kwargs):
  params_file = LaunchConfiguration('params_file')
  map_yaml_file = LaunchConfiguration('costmap_file')
  lifecycle_nodes = ['map_server', 'amcl']

  remappings = [('/tf', 'tf'),
                ('/tf_static', 'tf_static')]

  param_substitutions = {
    'yaml_filename': map_yaml_file
  }

  configured_params = ParameterFile(
    RewrittenYaml(
        source_file=params_file,
        root_key='',
        param_rewrites=param_substitutions,
        convert_types=True),
    allow_substs=True
  )

  map_odom_transformation = Node(
    package='tf2_ros',
    executable='static_transform_publisher',
    name='base_to_camera_tf',
    arguments=[
        '--x', '0.0',
        '--y', '0.0',
        '--z', '0.0',
        '--roll', '0.0',
        '--pitch', '0.0',
        '--yaw', '0.0',
        '--frame-id', 'map',
        '--child-frame-id', 'odom'
    ],
    output='screen'
  )

  load_composable_nodes = LoadComposableNodes(
    target_container='nav2_container',
    composable_node_descriptions=[
      ComposableNode(
          package='nav2_map_server',
          plugin='nav2_map_server::MapServer',
          name='map_server',
          parameters=[configured_params],
          remappings=remappings),
      ComposableNode(
          package='nav2_amcl',
          plugin='nav2_amcl::AmclNode',
          name='amcl',
          parameters=[configured_params],
          remappings=remappings),
      ComposableNode(
          package='nav2_lifecycle_manager',
          plugin='nav2_lifecycle_manager::LifecycleManager',
          name='lifecycle_manager_localization',
          parameters=[{'use_sim_time': 'true',
                        'autostart': 'true',
                        'node_names': lifecycle_nodes}]),
    ],
  )

  return [map_odom_transformation, load_composable_nodes]


def generate_launch_description():
  ld = LaunchDescription(ARGUMENTS)
  ld.add_action(OpaqueFunction(function=evaluate_spawn))
  return ld