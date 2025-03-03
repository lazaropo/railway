#pragma once

#include <UnigineComponentSystem.h>

#include "Carriage.h"

class Train : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(Train, Unigine::ComponentBase);

  PROP_PARAM(Float, start_speed, 10.f);
  PROP_PARAM(Float, max_speed, 40.f);
  PROP_PARAM(Float, margin, 1.f);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  void moveBy();

  Carriage::MOVE_DIRECTION getMoveDirection() const { return m_move_direction; }

  void changeMoveDirection() {
    m_is_stop = false;
    if (m_move_direction == Carriage::MOVE_DIRECTION::FORWARD) {
      m_move_direction = Carriage::MOVE_DIRECTION::REVERSE;
      m_position = (*m_carriage.back())
                       ->getSegmentPosition(Carriage::MOVE_DIRECTION::REVERSE);
    } else {
      m_move_direction = Carriage::MOVE_DIRECTION::FORWARD;
      m_position = (*m_carriage.front())
                       ->getSegmentPosition(Carriage::MOVE_DIRECTION::FORWARD);
    }
  }

  void brake() {
    m_speed = m_speed - m_speed_delta > Unigine::Math::Consts::EPS
                  ? m_speed - m_speed_delta
                  : 0.f;
  }

  void accelerate() {
    m_speed = m_speed + m_speed_delta < max_speed ? m_speed + m_speed_delta
                                                  : max_speed;
  }

  // void startMove() { m_is_stop = false; }

  Unigine::Math::Vec3 getFrontWorldPosition() const {
    return (*m_carriage.front())->getWorldPosition();
  }

  Unigine::Math::Vec3 getBackWorldPosition() const {
    return (*m_carriage.back())->getWorldPosition();
  }

  void setStartPosition(Unigine::SplineSegmentPtr segment,
                        float t_coordinate = 0);

 protected:
  void init();
  void update();

  bool m_is_stop = false;
  float m_speed = start_speed;
  float m_speed_delta = 1.f;

  Unigine::Vector<Carriage*> m_carriage;
  float m_carraige_len;
  Carriage::MOVE_DIRECTION m_move_direction = Carriage::FORWARD;

  SegmentPosition m_position;
};
