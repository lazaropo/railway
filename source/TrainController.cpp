#include "TrainController.h"

REGISTER_COMPONENT(TrainController);

using namespace Unigine;

void TrainController::init() {
  Vector<Train*> trains;
  ComponentSystem::get()->getComponentsInChildren<Train>(getNode(), trains);

  for (auto it : trains) {
    if ((forward_bogie = it->forward_bogie) && (back_bogie = it->back_bogie) &&
        (car_body = it->car_body)) {
      Log::message("Train load successful.\n", "\n");
      break;
    }
  }
}

void TrainController::update() {
  moveTrain();
  Train::update();
}

void TrainController::moveTrain() {
  if (!m_curr_segment) return;

  float ifps = Game::getIFps();
  Math::Mat4 transform =
      makeBogieTransform(forward_bogie, m_bogie_pos_forward, ifps);
  if (transform != Math::Mat4_zero) forward_bogie->setWorldTransform(transform);

  transform = makeBogieTransform(back_bogie, m_bogie_pos_back, ifps);
  if (transform != Math::Mat4_zero) back_bogie->setWorldTransform(transform);

  setCarBody(car_body);
}

void TrainController::setCarBody(NodePtr body) {
  if (!body) return;

  Math::Mat4 transform = body->getWorldTransform();

  Math::Vec3 forward_bogie_pos = forward_bogie->getWorldPosition();
  Math::Vec3 back_bogie_pos = back_bogie->getWorldPosition();

  Math::Vec3 pos = (back_bogie_pos + back_bogie_pos) / 2.f;

  body->setWorldPosition(pos);

  // forward_bogie->getWorldRotation();
}

Math::Mat4 TrainController::makeBogieTransform(const NodePtr node, BogiePos pos,
                                               float ifps) {
  if (!node) return Math::Mat4_zero;

  Math::Mat4 transform = node->getWorldTransform();
  Math::vec3 direction = node->getDirection();

  float relative_velocity =
      m_current_linear_velocity * ifps / pos.m_curr_segment_len;

  while (m_current_move_direction == MOVE_DIRECTION::FORWARD &&
         (pos.m_t_coordinate + relative_velocity >= 1.f)) {
    if (!pos.m_curr_segment) return Math::Mat4_zero;

    pos.m_t_coordinate += relative_velocity - 1.f;
    pos.m_curr_segment = m_callback_next_segment_f(pos.m_curr_segment);
    pos.m_curr_segment_len = pos.m_curr_segment->getLength();
  }

  while (m_current_move_direction == MOVE_DIRECTION::REVERSE &&
         (pos.m_t_coordinate - relative_velocity <= 0.f)) {
    if (!pos.m_curr_segment) return Math::Mat4_zero;

    pos.m_t_coordinate -= relative_velocity + 1.f;
    pos.m_curr_segment = m_callback_next_segment_f(pos.m_curr_segment);
    pos.m_curr_segment_len = pos.m_curr_segment->getLength();
  }

  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    pos.m_t_coordinate += relative_velocity;
  else
    pos.m_t_coordinate -= relative_velocity;

  Math::Vec3 new_pos =
      Math::Vec3(pos.m_curr_segment->calcPoint(pos.m_t_coordinate)) +
      Math::Vec3(0, 0, 3.f);
  Math::vec3 new_direction =
      pos.m_curr_segment->calcTangent(pos.m_t_coordinate);

  // Math::Vec3 new_pos = new_pos + transform.getColumn3(3);

  transform.setRotateZ(Math::getAngle(direction, new_direction));
  transform.setTranslate(new_pos - transform.getColumn3(3));

  Visualizer::renderDirection(new_pos, new_direction, Math::vec4_blue);

  return Math::Mat4_zero;
}

Math::Mat4 TrainController::calcNewPosition() {}
