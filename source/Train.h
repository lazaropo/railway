#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <UniginePrimitives.h>  // Box primitive - m_car_body
#include <UnigineVisualizer.h>
#include <UnigineWorlds.h>

#include "InputController.h"

class Train : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(Train, Unigine::ComponentBase);

  PROP_PARAM(Float, start_speed, 20.f);
  PROP_PARAM(Float, max_speed, 40.f);

  PROP_PARAM(Node, forward_bogey);
  PROP_PARAM(Node, back_bogey);
  PROP_PARAM(Node, car_body);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

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

  template <class T>
  T takeNext(T current, T pos, T delta);

  void setAcceleration(float value) {
    m_acceleration = Unigine::Math::clamp(value, -1.f, 1.f);
  }

  MOVE_DIRECTION getMoveDirection() { return m_current_move_direction; }
  void changeMoveDirection();

  virtual void moveTrain() = 0;

  void setSegment(Unigine::SplineSegmentPtr curr_segment,
                  Unigine::SplineSegmentPtr next_segment);
  Unigine::SplineSegmentPtr getCurrentSegment() { return m_curr_segment; }

  MOVE moveNode();

 private:
  void moveNode(const Unigine::Math::Vec3& pos,
                const Unigine::Math::vec3& angle);

 private:
  inline static int m_count = 0;
  // Unigine::ObjectMeshDynamicPtr m_forward_bogey;
  // Unigine::ObjectMeshDynamicPtr m_back_bogey;
  // Unigine::ObjectMeshDynamicPtr m_car_body;
  float m_current_linear_velocity = start_speed;
  float m_new_linear_velocity = start_speed;
  float m_acceleration = 0.f;

  MOVE_DIRECTION m_current_move_direction = MOVE_DIRECTION::FORWARD;

  float m_t_coordinate = 0;
  Unigine::SplineSegmentPtr m_prev_segment;
  Unigine::SplineSegmentPtr m_curr_segment;

  float m_curr_segment_len = 0;
  float m_excess_len = 0;

 protected:
  void init();
  void update();
};
