import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import GroupAction, IncludeLaunchDescription, DeclareLaunchArgument, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.conditions import IfCondition
from launch.substitutions import PythonExpression, LaunchConfiguration

from launch_ros.actions import SetRemap, Node, SetParameter, LoadComposableNodes
from launch_ros.descriptions import ComposableNode, ParameterFile
from nav2_common.launch import RewrittenYaml

current_package_share = get_package_share_directory('test_environment')

ARGUMENTS = [
  DeclareLaunchArgument('params_file', default_value=os.path.join(current_package_share, 'config', 'navigation', 'nav.yaml'),
    description='Full path to the ROS2 parameters file to use for all launched nodes'),
  DeclareLaunchArgument('graph_file', default_value=os.path.join(current_package_share, 'maps', 'level1.json'),
    description='Full path to the ROS2 graph .json file'),
]

def launch(context, *args, **kwargs):
    params_file = LaunchConfiguration('params_file')
    graph_file = LaunchConfiguration('graph_file')

    lifecycle_nodes = ['controller_server',
                       'smoother_server',
                       'planner_server',
                       'route_server',
                       'behavior_server',
                       'bt_navigator',
                       'waypoint_follower',
                       'velocity_smoother']

    remappings = [('/tf', 'tf'),
                  ('/tf_static', 'tf_static')]

    param_substitutions = {
      'graph_filepath': graph_file
    }

    configured_params = ParameterFile(
      RewrittenYaml(
          source_file=params_file,
          root_key='',
          param_rewrites=param_substitutions,
          convert_types=True),
      allow_substs=True
    )

    load_composable_nodes = LoadComposableNodes(
      target_container='nav2_container',
      composable_node_descriptions=[
        ComposableNode(
            package='nav2_controller',
            plugin='nav2_controller::ControllerServer',
            name='controller_server',
            parameters=[configured_params],
            remappings=remappings + [('cmd_vel', 'cmd_vel_nav')]),
        ComposableNode(
            package='nav2_smoother',
            plugin='nav2_smoother::SmootherServer',
            name='smoother_server',
            parameters=[configured_params],
            remappings=remappings),
        ComposableNode(
            package='nav2_route',
            plugin='nav2_route::RouteServer',
            name='route_server',
            parameters=[configured_params],
            remappings=remappings + [('plan', 'route_plan')]),
        ComposableNode(
            package='nav2_planner',
            plugin='nav2_planner::PlannerServer',
            name='planner_server',
            parameters=[configured_params],
            remappings=remappings),
        ComposableNode(
            package='nav2_behaviors',
            plugin='behavior_server::BehaviorServer',
            name='behavior_server',
            parameters=[configured_params],
            remappings=remappings),
        ComposableNode(
            package='nav2_bt_navigator',
            plugin='nav2_bt_navigator::BtNavigator',
            name='bt_navigator',
            parameters=[configured_params],
            remappings=remappings),
        ComposableNode(
            package='nav2_waypoint_follower',
            plugin='nav2_waypoint_follower::WaypointFollower',
            name='waypoint_follower',
            parameters=[configured_params],
            remappings=remappings),
        ComposableNode(
            package='nav2_velocity_smoother',
            plugin='nav2_velocity_smoother::VelocitySmoother',
            name='velocity_smoother',
            parameters=[configured_params],
            remappings=remappings +
                        [('cmd_vel', 'cmd_vel_nav'), ('cmd_vel_smoothed', 'cmd_vel')]),
        ComposableNode(
            package='nav2_lifecycle_manager',
            plugin='nav2_lifecycle_manager::LifecycleManager',
            name='lifecycle_manager_navigation',
            parameters=[{'use_sim_time': True,
                            'autostart': True,
                            'node_names': lifecycle_nodes}]),
      ],
    )


    return [load_composable_nodes]

def generate_launch_description():
  ld = LaunchDescription(ARGUMENTS)
  ld.add_action(SetParameter(name='use_sim_time', value = True))
  ld.add_action(OpaqueFunction(function=launch))
  return ld