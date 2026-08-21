class MovePolicy {
public:
  using MPoint = Eigen::Vector2i; // map point
  using WPoint = Eigen::Vector2d; // world point
  using RPoint = geometry_msgs::msg::PoseStamped; // ros point

public:
  virtual void update(const RPoint& src, const RPoint& dest) = 0;
  virtual bool hasMove() const = 0;
  virtual MPoint nextMove() = 0;
};

class BasicMovePolicy : public MovePolicy {
public:
  void update(const RPoint& src, const RPoint& dest) override { last_order = disp.getDirections(src, dest); }
  bool hasMove() const override { return move_idx < 4; }
  MPoint nextMove() override { return utils::getMove(last_order[move_idx++]); }

private:
  int move_idx = 0;
  std::array<utils::Move, 4> last_order;
  utils::TraverseDispatcher disp;
};

class DiagonalPolicy : public MovePolicy {
public:
  void update(const RPoint& src, const RPoint& dest) override { last_order = disp.getDirections(src, dest); }
  bool hasMove() const override { return move_idx < 8; }
  MPoint nextMove() override { return utils::getMove(last_order[move_idx++]); }

private:
  int move_idx = 0;
  std::array<utils::Move, 8> last_order;
  utils::TraverseDispatcher<utils::diagonal_move_tag> disp;
};