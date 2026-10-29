import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.conditions import IfCondition
from launch.substitutions import PythonExpression, LaunchConfiguration

from launch_ros.actions import Node, SetParameter

ARGUMENTS = [
	DeclareLaunchArgument('slam_algo', default_value='toolbox',
												choices = ['toolbox', 'rtabmap'],
												description='slam solver for robot')
]


def launch(context, *args, **kwargs):
	# constant's
	current_package_name = 'test_environment'

	slam_type = LaunchConfiguration('slam_algo').perform(context)

	# config's path
	rtabmap_config_path = os.path.join(get_package_share_directory(current_package_name), 'config', 'navigation', 'rtabmap.yaml')
	slam_toolbox_config_path = os.path.join(get_package_share_directory(current_package_name), 'config', 'navigation', 'toolbox.yaml')

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

	return [rtabmap_slam, slam_toolbox]

def generate_launch_description():
	ld = LaunchDescription(ARGUMENTS)
	ld.add_action(SetParameter(name='use_sim_time', value = True))
	ld.add_action(OpaqueFunction(function=launch))
	return ld