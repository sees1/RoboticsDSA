import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import GroupAction, IncludeLaunchDescription, DeclareLaunchArgument, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.conditions import IfCondition
from launch.substitutions import PythonExpression, LaunchConfiguration

from launch_ros.actions import SetRemap, Node, SetParameter

ARGUMENTS = [
    DeclareLaunchArgument('slam_type', default_value='toolbox',
                          choices = ['toolbox', 'rtabmap'],
                          description='slam solver for robot')
]

def launch(context, *args, **kwargs):
    # constant's
    current_package_name = 'test_environment'
    nav2_bringup_package_name = 'nav2_bringup'

    slam_type = LaunchConfiguration('slam_type').perform(context)

    # config's path
    nav2_params_path = os.path.join(get_package_share_directory(current_package_name), 'config', 'navigation', 'nav.yaml')
    rtabmap_config_path = os.path.join(get_package_share_directory(current_package_name), 'config', 'navigation', 'rtabmap.yaml')
    slam_toolbox_config_path = os.path.join(get_package_share_directory(current_package_name), 'config', 'navigation', 'toolbox.yaml')

    # launch's path
    nav2_launch_path = os.path.join(get_package_share_directory(nav2_bringup_package_name), 'launch', 'navigation_launch.py')

    # navigation stack ver.2 start
    nav2 = GroupAction([
        SetRemap('/global_costmap/scan', '/scan'),
        SetRemap('/local_costmap/scan', '/scan'),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(nav2_launch_path),
            launch_arguments=[('use_sim_time', 'true'),
                              ('params_file', nav2_params_path),
                              ('use_composition', 'False'),
                              ('namespace', '')]
        ),
    ])

    rtabmap_slam = Node(
        package='rtabmap_slam',
        executable='rtabmap',
        name="rtabmap",
        output="screen",
        parameters=[rtabmap_config_path],
        remappings=[
            ("/grid_prob_map", "/map"),
            ("scan_cloud", "/camera/points"),
            ("rgb/image", "/camera/image_raw"),
            ("rgb/camera_info", "/camera/camera_info"),
            ("depth/image", "/camera/depth/image_raw")
        ],
        arguments=["--delete_db_on_start"],
        condition=IfCondition(PythonExpression([repr(slam_type), " == 'rtabmap'"]))
    )

    slam_toolbox = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(get_package_share_directory('slam_toolbox'), 'launch', 'online_async_launch.py')),
        launch_arguments={
            'slam_params_file': slam_toolbox_config_path,
            'use_sim_time': 'true',
            'autostart': 'true',
        }.items(),
        condition=IfCondition(PythonExpression([repr(slam_type), " == 'toolbox'"]))
    )

    return [nav2, rtabmap_slam, slam_toolbox]

def generate_launch_description():
    ld = LaunchDescription(ARGUMENTS)
    ld.add_action(SetParameter(name='use_sim_time', value = True))
    ld.add_action(OpaqueFunction(function=launch))
    return ld