namespace utils {
  struct diagonal_move_tag;

  using MPoint = Eigen::Vector2i; // map point
  using WPoint = Eigen::Vector2d; // world point
  using RPoint = geometry_msgs::msg::PoseStamped; // ros point
  using Transform = geometry_msgs::msg::TransformStamped;

  WPoint flatenizePoint(const RPoint& point) { return {point.pose.position.x, point.pose.position.y}; }
  
  enum Move { UP = 0, RIGHT, DOWN, LEFT, UPRIGHT, DOWNRIGHT, DOWNLEFT, UPLEFT };
  enum Quadrant { TOPRIGHT = 0, TOPLEFT, BOTTOMLEFT, BOTTOMRIGHT };
    
  Quadrant findQuadrant(const RPoint& src, const RPoint& dest)
  {
    if (src.pose.position.x < dest.pose.position.x) 
    {
      if (src.pose.position.y < dest.pose.position.y)
        return Quadrant::BOTTOMLEFT;
      else
        return Quadrant::TOPLEFT;
    }
    else
    {
      if (src.pose.position.y < dest.pose.position.y)
        return Quadrant::BOTTOMRIGHT;
      else
        return Quadrant::TOPRIGHT;
    }
  }

  MPoint getMove(Move m)
  {
    switch(m)
    {
      case UP:
        return {0, 1};
      case RIGHT:
        return {1, 0};
      case DOWN:
        return {0, -1};
      case LEFT:
        return {-1, 0};
      case UPRIGHT:
        return {1, 1};
      case DOWNRIGHT:
        return {1, -1};
      case DOWNLEFT:
        return {-1, -1};
      case UPLEFT:
        return {-1, 1};
      default:
        throw std::runtime_error("getMove: unable to find such move");
    }
  }

  // DOC: main purpose - prioritize movements by direction
  template <typename Tag = void>
  struct TraverseDispatcher {
    std::array<Move, 4> getDirections(const RPoint& src, const RPoint& dest)
    {
      double diff_x = std::abs(dest.pose.position.x - src.pose.position.x);
      double diff_y = std::abs(dest.pose.position.y - src.pose.position.y);

      bool y_prior = diff_x < diff_y;

      return directions[findQuadrant(src, dest) * 2 + y_prior];
    }

  private:
    std::array<std::array<Move, 4>, 8> directions = 
    {
      // even idx - diffx > diffy
      // odd idx - diffx < diffy
      {LEFT, DOWN, RIGHT, UP}, // TOPRIGHT
      {DOWN, LEFT, UP, RIGHT},
      {RIGHT, DOWN, LEFT, UP}, // TOPLEFT
      {DOWN, RIGHT, UP, LEFT},
      {RIGHT, UP, LEFT, DOWN}, // BOTTOMLEFT
      {UP, RIGHT, DOWN, LEFT},
      {LEFT, UP, RIGHT, DOWN}, // BOTTOMRIGHT
      {UP, LEFT, DOWN, RIGHT}
    };
  };

  template <>
  struct TraverseDispatcher<diagonal_move_tag> {
    std::array<Move, 8> getDirections(const RPoint& src, const RPoint& dest)
    {
      double diff_x = std::abs(dest.pose.position.x - src.pose.position.x);
      double diff_y = std::abs(dest.pose.position.y - src.pose.position.y);

      bool y_prior = diff_x < diff_y;

      return directions[findQuadrant(src, dest) * 2 + y_prior];
    }

  private:
    std::array<std::array<Move, 8>, 8> directions = 
    {
      // even idx - diffx > diffy
      // odd idx - diffx < diffy
      { LEFT,  DOWNLEFT,  DOWN,   UPLEFT,    RIGHT, DOWNRIGHT, UP,    UPRIGHT }, // TOPRIGHT
      { DOWN,  DOWNLEFT,  LEFT,   UPLEFT,    UP,    DOWNRIGHT, RIGHT, UPRIGHT },
      { RIGHT, DOWNRIGHT, DOWN,   UPRIGHT,   LEFT,  DOWNLEFT,  UP,    UPLEFT }, // TOPLEFT
      { DOWN,  DOWNRIGHT, RIGHT,  UPRIGHT,   UP,    DOWNLEFT,  LEFT,  UPLEFT },
      { RIGHT, UPRIGHT,   UP,     DOWNRIGHT, LEFT,  UPLEFT,    DOWN,  DOWNLEFT }, // BOTTOMLEFT
      { UP,    UPRIGHT,   RIGHT,  DOWNRIGHT, DOWN,  UPLEFT,    LEFT,  DOWNLEFT },
      { LEFT,  UPLEFT,    UP,     DOWNLEFT,  RIGHT, UPRIGHT,   DOWN,  DOWNRIGHT }, // BOTTOMRIGHT
      { UP,    UPLEFT,    LEFT,   UPRIGHT,   DOWN,  DOWNLEFT,  RIGHT, DOWNRIGHT },
    };
  };

} // namespace utils
