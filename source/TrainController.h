#pragma once

#include <UnigineComponentSystem.h>

#include <functional>

#include "Train.h"
// #include "TrainManager.h"

class TrainController : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(TrainController, Unigine::ComponentBase);

  PROP_PARAM(Float, start_speed, 20.f);
  PROP_PARAM(Float, max_speed, 40.f);

  // PROP_PARAM(Node, train_node);

  enum MOVE {
    SUCCESS,  // node is moved
    END,      // end of current segment
    STOP,     // end of this segments path
    FAIL      // fails at move function
  };

  enum MOVE_DIRECTION {
    FORWARD,
    REVERSE,
  };

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  void setSegment(Unigine::SplineSegmentPtr curr_segment, float pos = 0.f);

  void setPrevSegmentFunction(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
    m_callback_prev_segment_f = fp;
  }

  void setAcceleration(float value) {
    m_acceleration = Unigine::Math::clamp(value, -1.f, 1.f);
  }

  MOVE_DIRECTION getMoveDirection() { return m_current_move_direction; }
  void changeMoveDirection();
  float getLength() const {
    if (m_car)
      return m_bogie_distance;
    else
      return 0.f;
  }
  Unigine::SplineSegmentPtr getCurrentSegment() { return m_curr_segment; }

 protected:
  struct BogiePos {
    Unigine::SplineSegmentPtr m_curr_segment;
    float m_t_coordinate = 0;
    float m_curr_segment_len = 0;
  };

  template <class T>
  T takeNext(T current, T pos, T delta);

  void moveTrain();

  void renderNode(Unigine::NodePtr node) {
    Unigine::Math::Vec3 pos = node->getPosition();
    Unigine::Visualizer::renderVector(
        pos,
        pos + (Unigine::Math::Vec3)m_car->getDirection(Unigine::Math::AXIS_X),
        Unigine::Math::vec4_red);
    Unigine::Visualizer::renderVector(
        pos,
        pos + (Unigine::Math::Vec3)m_car->getDirection(Unigine::Math::AXIS_Y),
        Unigine::Math::vec4_green);
    Unigine::Visualizer::renderVector(
        pos,
        pos + (Unigine::Math::Vec3)m_car->getDirection(Unigine::Math::AXIS_Z),
        Unigine::Math::vec4_blue);
  }

 private:
  // void moveBogie(Unigine::NodePtr bogie, float ifps);
  void setCarBody(Unigine::NodePtr body);

  Unigine::Math::Mat4 makeBogieTransform(const Unigine::NodePtr node,
                                         BogiePos pos, float ifps);

  Unigine::Math::Mat4 calcNewPosition();

  float calcNextPos();

  void init();
  void update();

 protected:
  inline static int m_count = 0;
  // Unigine::ObjectMeshDynamicPtr m_forward_bogey;
  // Unigine::ObjectMeshDynamicPtr m_back_bogey;
  // Unigine::ObjectMeshDynamicPtr m_car_body;
  float m_current_linear_velocity = start_speed;
  float m_new_linear_velocity = start_speed;
  float m_acceleration = 0.f;

  float m_bogie_distance = 0.f;

  MOVE_DIRECTION m_current_move_direction = MOVE_DIRECTION::FORWARD;

  // Unigine::SplineSegmentPtr m_prev_segment;
  Unigine::SplineSegmentPtr m_curr_segment;

  Unigine::NodePtr m_forward_bogie;
  Unigine::NodePtr m_back_bogie;
  Unigine::NodePtr m_car;

  BogiePos m_bogie_pos_forward;
  BogiePos m_bogie_pos_back;
  BogiePos m_car_pos;

  inline static std::function<Unigine::SplineSegmentPtr(
      Unigine::SplineSegmentPtr)>
      m_callback_prev_segment_f;
};
