

using WPoint = Eigen::Vector2d; // world point

struct ANode {
  size_t G = 0;
  size_t H = 0;
  WPoint coord;
  ANode* parent;  
};

class AStar : public Base {
public:
  using map_type = Costmap2D;

public:
  using Path = nav_msgs::msg::Path;
  using Pose = geometry_msgs::msg::PoseStamped;

public:
  AStar(std::unique_ptr<MovePolicy> policy)
  : Base(std::move(policy))
  {

  }

  nav_msgs::msg::Path createPlan(const geometry_msgs::msg::PoseStamped & start,
                                 const geometry_msgs::msg::PoseStamped & goal);

private:
  std::unique_ptr<MovePolicy> policy_;
};