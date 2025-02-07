#include "InputController.h"

REGISTER_COMPONENT(InputController);

using namespace Unigine;

InputController* InputController::m_instance = nullptr;
bool InputController::m_is_enabled = false;
bool InputController::m_is_action = false;

void InputController::init() {
  m_instance = this;
  m_is_enabled = true;
  m_is_action = false;
}

void InputController::update() {
  m_is_action = false;

  for (InputAction it : m_actions)
    if (it.update()) m_is_action = true;
}

float InputController::getActionState(INPUT_ACTIONS action) {
  if (!m_instance || !m_is_enabled) return 0.f;

  m_instance->m_actions[(int)action].update();

  return m_instance->m_actions[(int)action].m_state;
}

// void Train::setSegment(SplineSegmentPtr curr_segment,
//                        SplineSegmentPtr next_segment) {
//   m_curr_segment = curr_segment;
//   m_next_segment = next_segment;

//   m_t_coordinate = 0.f;
//   if (m_excess_len > Math::Consts::EPS) {
//     m_t_coordinate = m_excess_len;
//     m_excess_len = 0;
//   }

//   getNode()->setPosition(Math::Vec3(m_curr_segment->calcPoint(m_t_coordinate)));
//   m_curr_segment_len = m_curr_segment->getLength();
// }

// Train::MOVE Train::moveNode() {
//   float move_path = speed * Game::getIFps() / m_curr_segment_len;
//   if (move_path + m_t_coordinate >= 1.f) {
//     if (!m_next_segment) return MOVE::STOP;

//     m_t_coordinate += move_path - 1.f;
//     m_excess_len = m_t_coordinate;

//     Math::Vec3 v_pos =
//         Math::Vec3(m_curr_segment->getEndPoint()->getPosition()) +
//         Math::Vec3(0, 0, 3.f);
//     Math::vec3 v_tanget = m_curr_segment->getEndTangent();

//     moveNode(v_pos, v_tanget);

//     Visualizer::renderDirection(v_pos, v_tanget, Math::vec4_blue);
//     return MOVE::END;
//   } else {
//     m_t_coordinate += move_path;
//     Math::Vec3 v_pos = Math::Vec3(m_curr_segment->calcPoint(m_t_coordinate))
//     +
//                        Math::Vec3(0, 0, 3.f);
//     Math::vec3 v_tanget = m_curr_segment->calcTangent(m_t_coordinate);

//     moveNode(v_pos, v_tanget);

//     Visualizer::renderDirection(v_pos, v_tanget, Math::vec4_blue);
//     return MOVE::SUCCESS;
//   }

//   return MOVE::FAIL;
// }

// void Train::moveNode(const Math::Vec3& pos, const Unigine::Math::vec3& angle)
// {
//   NodePtr my_node = getNode();

//   my_node->setWorldDirection(angle, Math::vec3_up);
//   my_node->setWorldPosition(pos);

//   Visualizer::renderDirection(
//       pos, Math::vec3(my_node->getTransform().getAxisX()), Math::vec4_red);
//   Visualizer::renderDirection(
//       pos, Math::vec3(my_node->getTransform().getAxisY()), Math::vec4_green);
//   Visualizer::renderDirection(
//       pos, Math::vec3(my_node->getTransform().getAxisZ()), Math::vec4_black);
// }
