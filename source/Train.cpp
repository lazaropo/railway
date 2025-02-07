#include "Train.h"

REGISTER_COMPONENT(Train);

using namespace Unigine;

template <class T>
T Train::takeNext(T current, T new_value, T delta) {
  if (Math::abs(new_value - current) <= delta) return new_value;

  return current + Math::sign(new_value - current) * delta;
}

void Train::init() {
  if (start_speed > max_speed) {
    m_current_linear_velocity = max_speed;
    m_new_linear_velocity = max_speed;

    start_speed = max_speed;
  }
  // if (wheel_fl)
  // 	joint_wheel_fl =
  // checked_ptr_cast<JointWheel>(wheel_fl->getObjectBody()->getJoint(0));

  // if (wheel_fr)
  // 	joint_wheel_fr =
  // checked_ptr_cast<JointWheel>(wheel_fr->getObjectBody()->getJoint(0));

  // carBodyRigid = node->getObjectBodyRigid();
}

void Train::update() {
  float ifps = Game::getIFps();

  m_current_linear_velocity = takeNext(
      m_current_linear_velocity, m_new_linear_velocity, start_speed * ifps);
  m_current_linear_velocity =
      Math::clamp(m_current_linear_velocity, 0.f, max_speed);
}

void Train::changeMoveDirection() {
  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    m_current_move_direction = MOVE_DIRECTION::REVERSE;
  else
    m_current_move_direction = MOVE_DIRECTION::FORWARD;

  m_current_linear_velocity = start_speed;
}
