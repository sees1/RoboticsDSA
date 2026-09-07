#include "astar.hpp"



nav_msgs::msg::Path AStar::createPlan(const geometry_msgs::msg::PoseStamped & start,
                                      const geometry_msgs::msg::PoseStamped & goal)
{
  std::vector<Node*> open_list;
  std::vector<Node*> close_list;

  Node* node = new Node()
}