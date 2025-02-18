// #include "Train.h"

// REGISTER_COMPONENT(Train);

// using namespace Unigine;

// template <class T>
// T Train::takeNext(T current, T new_value, T delta) {
//   if (Math::abs(new_value - current) <= delta) return new_value;

//   return current + Math::sign(new_value - current) * delta;
// }

// void Train::init() {
//   if (start_speed > max_speed) {
//     m_current_linear_velocity = max_speed;
//     m_new_linear_velocity = max_speed;

//     start_speed = max_speed;
//   }

//   if (forward_bogie && back_bogie)
//     m_bogie_distance =
//         (forward_bogie->getPosition() - back_bogie->getPosition()).length();
// }

// void Train::update() {
//   // if(!forward_bogie || !back_bogie || !car_body) {
//   //   getNode().deleteLater();
//   //   return;
//   // }
//   if (Math::abs(m_bogie_distance) < Math::Consts::EPS && forward_bogie &&
//       back_bogie)
//     m_bogie_distance =
//         (forward_bogie->getPosition() - back_bogie->getPosition()).length();

//   float ifps = Game::getIFps();

//   m_current_linear_velocity = takeNext(
//       m_current_linear_velocity, m_new_linear_velocity, start_speed * ifps);
//   m_current_linear_velocity =
//       Math::clamp(m_current_linear_velocity, 0.f, max_speed);

//   // Math::Mat4 transform = forward_bogie->getTransform();
//   // transform.setTranslate()
//   forward_bogie->setPosition(m_bogie_pos_forward.m_curr_segment->calcPoint(
//       m_bogie_pos_forward.m_t_coordinate));
//   forward_bogie->setDirection(m_bogie_pos_forward.m_curr_segment->calcTangent(
//                                   m_bogie_pos_forward.m_t_coordinate),
//                               Math::vec3_up, Math::AXIS_X);

//   car_body->setPosition(
//       m_car_pos.m_curr_segment->calcPoint(m_car_pos.m_t_coordinate));
//   car_body->setDirection(
//       m_car_pos.m_curr_segment->calcTangent(m_car_pos.m_t_coordinate),
//       Math::vec3_up, Math::AXIS_X);

//   back_bogie->setPosition(m_bogie_pos_back.m_curr_segment->calcPoint(
//       m_bogie_pos_back.m_t_coordinate));
//   back_bogie->setDirection(m_bogie_pos_back.m_curr_segment->calcTangent(
//                                m_bogie_pos_back.m_t_coordinate),
//                            Math::vec3_up, Math::AXIS_X);

//   auto pos = getNode()->getPosition();
//   Visualizer::renderVector(pos, pos + getNode()->getTransform().getAxisX(),
//                            Math::vec4_red);
//   Visualizer::renderVector(pos, pos + getNode()->getTransform().getAxisY(),
//                            Math::vec4_green);
//   Visualizer::renderVector(pos, pos + getNode()->getTransform().getAxisZ(),
//                            Math::vec4_blue);

//   Visualizer::renderVector(pos, pos + (Math::Vec3)getNode()->getDirection(),
//                            Math::vec4_black);
// }

// void Train::changeMoveDirection() {
//   if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
//     m_current_move_direction = MOVE_DIRECTION::REVERSE;
//   else
//     m_current_move_direction = MOVE_DIRECTION::FORWARD;

//   m_current_linear_velocity = start_speed;
// }
