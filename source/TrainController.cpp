#include "TrainController.h"

REGISTER_COMPONENT(TrainController);

using namespace Unigine;

void TrainController::init() {
  NodePtr train_node = getNode();

  NodePtr node = train_node->findNode("BogieFront", true);
  m_forward_bogie_node = node;

  node = getNode()->findNode("BogieBack", true);
  m_back_bogie_node = node;

  node = getNode()->findNode("Body", true);
  m_car_node = node;

  if (start_speed > max_speed) {
    m_current_linear_velocity = max_speed;
    m_new_linear_velocity = max_speed;

    start_speed = max_speed;
  }

  if (m_forward_bogie_node && m_back_bogie_node)
    m_bogie_distance =
        (m_forward_bogie_node->getPosition() - m_back_bogie_node->getPosition())
            .length();
}

void TrainController::update() {
  if (!m_forward_bogie_node || !m_back_bogie_node || !m_car_node) return;

  if (m_is_stop) return;

  if (Math::abs(m_bogie_distance) < Math::Consts::EPS && m_forward_bogie_node &&
      m_back_bogie_node)
    m_bogie_distance =
        (m_forward_bogie_node->getPosition() - m_back_bogie_node->getPosition())
            .length();

  moveTrain();

  if (!m_forward_bogie_pos.m_curr_segment || !m_back_bogie_pos.m_curr_segment)
    return;

  Math::Vec3 forward_pos = m_forward_bogie_pos.m_curr_segment->calcPoint(
      m_forward_bogie_pos.m_t_coordinate);
  Math::Vec3 back_pos = m_back_bogie_pos.m_curr_segment->calcPoint(
      m_back_bogie_pos.m_t_coordinate);
  Math::Vec3 car_pos = (forward_pos - back_pos) / 2 + back_pos;

  Math::Vec3 car_direction = forward_pos - back_pos;

  getNode()->setWorldDirection(Math::vec3(car_direction), Math::vec3_up,
                               Math::AXIS_Y);
  getNode()->setWorldPosition(car_pos);

  m_forward_bogie_node->setWorldDirection(
      m_forward_bogie_pos.m_curr_segment->calcTangent(
          m_forward_bogie_pos.m_t_coordinate),
      Math::vec3_up, Math::AXIS_Y);

  m_back_bogie_node->setWorldDirection(
      m_back_bogie_pos.m_curr_segment->calcTangent(
          m_back_bogie_pos.m_t_coordinate),
      Math::vec3_up, Math::AXIS_Y);

  renderNode(m_car_node);
  renderNode(m_forward_bogie_node);
}

float TrainController::getLength() const {
  if (m_car_node)
    return m_bogie_distance + 2 * TRAINS_MARGIN;
  else
    return 0.f;
}

void TrainController::changeMoveDirection() {
  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    m_current_move_direction = MOVE_DIRECTION::REVERSE;
  else
    m_current_move_direction = MOVE_DIRECTION::FORWARD;
}

void TrainController::moveTrain() {
  if (!m_forward_bogie_node || !m_back_bogie_node || !m_car_node) return;

  m_current_linear_velocity = m_new_linear_velocity;

  if (m_forward_bogie_pos.m_curr_segment &&
      m_current_move_direction == MOVE_DIRECTION::FORWARD) {
    BogiePos pos = m_movement_logic->moveFirstBogie(m_forward_bogie_pos,
                                                    m_current_linear_velocity);
    if (!pos.m_curr_segment)
      m_callback_move_stop();
    else
      m_forward_bogie_pos = pos;

    m_back_bogie_pos = m_movement_logic->calcSecondBogiePos(
        m_forward_bogie_pos, m_back_bogie_pos, m_bogie_distance);

  } else {
    BogiePos pos = m_movement_logic->moveFirstBogie(m_back_bogie_pos,
                                                    -m_current_linear_velocity);
    if (!pos.m_curr_segment)
      m_callback_move_stop();
    else
      m_back_bogie_pos = pos;

    m_forward_bogie_pos = m_movement_logic->calcSecondBogiePos(
        m_back_bogie_pos, m_forward_bogie_pos, -m_bogie_distance);
  }
}

void TrainController::renderNode(Unigine::NodePtr node) {
  Unigine::Math::Vec3 pos = node->getWorldPosition();
  Unigine::Visualizer::renderVector(
      pos,
      pos + (Unigine::Math::Vec3)m_car_node->getWorldDirection(
                Unigine::Math::AXIS_X),
      Unigine::Math::vec4_red);
  Unigine::Visualizer::renderVector(
      pos,
      pos + (Unigine::Math::Vec3)m_car_node->getWorldDirection(
                Unigine::Math::AXIS_Y),
      Unigine::Math::vec4_green);
  Unigine::Visualizer::renderVector(
      pos,
      pos + (Unigine::Math::Vec3)m_car_node->getWorldDirection(
                Unigine::Math::AXIS_Z),
      Unigine::Math::vec4_blue);
}

SplineSegmentPtr TrainController::getCurrentSegment() const {
  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    return m_forward_bogie_pos.m_curr_segment;
  else
    return m_back_bogie_pos.m_curr_segment;
}

float TrainController::getCurrentParamPos() const {
  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    return m_forward_bogie_pos.m_t_coordinate;
  else
    return m_back_bogie_pos.m_t_coordinate;
}

void TrainController::setTrainAtSegment(Unigine::SplineSegmentPtr curr_segment,
                                        Unigine::SplineSegmentPtr prev_segment,
                                        float pos) {
  if (!curr_segment) return;

  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    setSegmentForward(curr_segment, prev_segment, pos);
  else
    setSegmentReverse(curr_segment, prev_segment, pos);
}

void TrainController::setSegmentForward(Unigine::SplineSegmentPtr curr_segment,
                                        Unigine::SplineSegmentPtr prev_segment,
                                        float pos) {
  BogiePos& first_bogie = m_forward_bogie_pos;
  BogiePos& second_bogie = m_back_bogie_pos;

  first_bogie.m_curr_segment = curr_segment;

  if (Math::abs(m_bogie_distance) < Math::Consts::EPS) return;

  Unigine::SplineSegmentPtr tmp_segment;

  float len = curr_segment->getLength();

  first_bogie.m_t_coordinate = Math::clamp(pos, 0.f, 1.f);
  first_bogie.m_curr_segment_len = len;

  if (first_bogie.m_t_coordinate * len < m_bogie_distance) {
    tmp_segment = prev_segment;

    second_bogie.m_curr_segment = tmp_segment;
    second_bogie.m_curr_segment_len = tmp_segment->getLength();
    float back_t_pos =
        1.f - (m_bogie_distance - first_bogie.m_t_coordinate * len) /
                  second_bogie.m_curr_segment_len;
    second_bogie.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);

  } else {
    second_bogie.m_curr_segment = curr_segment;
    second_bogie.m_curr_segment_len = len;
    float back_t_pos = first_bogie.m_t_coordinate - m_bogie_distance / len;
    second_bogie.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);
  }
}
void TrainController::setSegmentReverse(Unigine::SplineSegmentPtr curr_segment,
                                        Unigine::SplineSegmentPtr prev_segment,
                                        float pos) {
  BogiePos& first_bogie = m_back_bogie_pos;
  BogiePos& second_bogie = m_forward_bogie_pos;

  first_bogie.m_curr_segment = curr_segment;

  if (Math::abs(m_bogie_distance) < Math::Consts::EPS) return;

  Unigine::SplineSegmentPtr tmp_segment;

  float len = curr_segment->getLength();

  first_bogie.m_t_coordinate = Math::clamp(pos, 0.f, 1.f);
  first_bogie.m_curr_segment_len = len;

  if ((1.f - first_bogie.m_t_coordinate) * len < m_bogie_distance) {
    tmp_segment = prev_segment;

    second_bogie.m_curr_segment = tmp_segment;
    second_bogie.m_curr_segment_len = tmp_segment->getLength();
    float back_t_pos =
        (m_bogie_distance - (1.f - first_bogie.m_t_coordinate) * len) /
        second_bogie.m_curr_segment_len;
    second_bogie.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);

  } else {
    second_bogie.m_curr_segment = curr_segment;
    second_bogie.m_curr_segment_len = len;
    float back_t_pos = first_bogie.m_t_coordinate + m_bogie_distance / len;
    second_bogie.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);
  }
}