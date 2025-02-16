#include "TrainController.h"

REGISTER_COMPONENT(TrainController);

using namespace Unigine;

void TrainController::init() {
  NodePtr train_node = getNode();

  NodePtr node = train_node->findNode("BogieFront", true);
  m_forward_bogie = node;

  node = getNode()->findNode("BogieBack", true);
  m_back_bogie = node;

  node = getNode()->findNode("Body", true);
  m_car = node;

  if (start_speed > max_speed) {
    m_current_linear_velocity = max_speed;
    m_new_linear_velocity = max_speed;

    start_speed = max_speed;
  }

  if (m_forward_bogie && m_back_bogie)
    m_bogie_distance =
        (m_forward_bogie->getPosition() - m_back_bogie->getPosition()).length();
}

void TrainController::update() {
  if (!m_forward_bogie || !m_back_bogie || !m_car) return;

  if (m_is_stop) return;

  if (Math::abs(m_bogie_distance) < Math::Consts::EPS && m_forward_bogie &&
      m_back_bogie)
    m_bogie_distance =
        (m_forward_bogie->getPosition() - m_back_bogie->getPosition()).length();

  // float ifps = Game::getIFps();

  moveTrain();

  if (!m_bogie_pos_forward.m_curr_segment || !m_bogie_pos_back.m_curr_segment)
    return;

  Math::Vec3 forward_pos = m_bogie_pos_forward.m_curr_segment->calcPoint(
      m_bogie_pos_forward.m_t_coordinate);
  Math::Vec3 back_pos = m_bogie_pos_back.m_curr_segment->calcPoint(
      m_bogie_pos_back.m_t_coordinate);
  Math::Vec3 car_pos = (forward_pos - back_pos) / 2 + back_pos;

  Math::Vec3 car_direction = forward_pos - back_pos;
  // if(m_current_move_direction == MOVE_DIRECTION::REVERSE){
  //   car_pos = - car_pos;
  //   car_direction = - car_direction;
  // }

  getNode()->setWorldDirection(Math::vec3(car_direction), Math::vec3_up,
                               Math::AXIS_Y);
  getNode()->setWorldPosition(car_pos);

  // m_forward_bogie->setWorldPosition(forward_pos);
  m_forward_bogie->setWorldDirection(
      m_bogie_pos_forward.m_curr_segment->calcTangent(
          m_bogie_pos_forward.m_t_coordinate),
      Math::vec3_up, Math::AXIS_Y);

  // m_car->setWorldPosition(car_pos);
  // m_car->setWorldDirection(car_direction, Math::vec3_up, Math::AXIS_Y);

  // m_back_bogie->setWorldPosition(back_pos);
  m_back_bogie->setWorldDirection(m_bogie_pos_back.m_curr_segment->calcTangent(
                                      m_bogie_pos_back.m_t_coordinate),
                                  // Math::vec3(m_forward_bogie->getPosition() -
                                  // m_back_bogie->getPosition()),
                                  Math::vec3_up, Math::AXIS_Y);

  renderNode(m_car);
  renderNode(m_forward_bogie);

  // Visualizer::renderVector(car_pos, car_direction, Math::vec4_black);
  // Visualizer::renderVector(getNode()->getWorldPosition(),
  //                          getNode()->getWorldTransform().getAxisX(),
  //                          Math::vec4_red);
  // Visualizer::renderVector(getNode()->getWorldPosition(),
  //                          getNode()->getWorldTransform().getAxisY(),
  //                          Math::vec4_green);
  // Visualizer::renderVector(getNode()->getWorldPosition(),
  //                          getNode()->getWorldTransform().getAxisZ(),
  //                          Math::vec4_blue);
  // Train::update();
}

void TrainController::shutdown() {
  // FilePtr fp = File::create();
  // if(fp->open("record.txt", "wb")){

  //   size_t sum = 0;
  //   for(auto it : m_count_collection) {
  //     sum += it;
  //     fp->writeInt(it);
  //     fp->writeChar(' ');
  //   }
  //   fp->writeInt(sum);

  //   fp->close();
  // }
  size_t sum = 0;
  size_t count_of_excess = 0;
  for (auto it : m_count_collection) {
    sum += it;
    if (it > 9) ++count_of_excess;
  }

  Log::message("\n%u - sum\n%u - size", sum, m_count_collection.size());
}

template <class T>
T TrainController::takeNext(T current, T new_value, T delta) {
  if (Math::abs(new_value - current) <= delta) return new_value;

  return current + Math::sign(new_value - current) * delta;
}

void TrainController::changeMoveDirection() {
  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    m_current_move_direction = MOVE_DIRECTION::REVERSE;
  else
    m_current_move_direction = MOVE_DIRECTION::FORWARD;

  // m_current_linear_velocity = start_speed;
}

void TrainController::moveTrain() {
  if (!m_forward_bogie || !m_back_bogie || !m_car || !m_callback_prev_segment_f)
    return;

  float ifps = Game::getIFps();

  m_current_linear_velocity = m_new_linear_velocity;

  if (m_current_move_direction == MOVE_DIRECTION::FORWARD) {
    float shift = m_bogie_pos_forward.m_t_coordinate +
                  m_current_linear_velocity * ifps /
                      m_bogie_pos_forward.m_curr_segment_len;
    m_prev_segment = m_bogie_pos_back.m_curr_segment;
    SplineSegmentPtr segment = getCurrentSegment();
    while (shift - 1.f > Math::Consts::EPS) {
      m_prev_segment = m_bogie_pos_forward.m_curr_segment;
      segment = m_callback_next_segment_f(segment);

      if (segment) {
        shift = (shift - 1.f) * m_bogie_pos_forward.m_curr_segment_len /
                segment->getLength();
        m_bogie_pos_forward.m_curr_segment = segment;
      } else {
        m_callback_move_stop();
        return;
        // segment = getCurrentSegment();
        // shift = 1.f;
      }
    }

    setSegment(segment, shift);
    m_bogie_pos_back = calcBackBogiePos(m_bogie_pos_forward, m_bogie_pos_back);
  } else {
    float shift =
        m_bogie_pos_back.m_t_coordinate -
        m_current_linear_velocity * ifps / m_bogie_pos_back.m_curr_segment_len;
    m_prev_segment = m_bogie_pos_forward.m_curr_segment;
    SplineSegmentPtr segment = getCurrentSegment();
    while (shift < 0.f) {
      m_prev_segment = m_bogie_pos_back.m_curr_segment;
      segment = m_callback_prev_segment_f(segment);
      if (segment) {
        shift = 1.f + (shift)*m_bogie_pos_back.m_curr_segment_len /
                          segment->getLength();
        m_bogie_pos_back.m_curr_segment = segment;
      } else {
        m_callback_move_stop();
        return;
        // segment = getCurrentSegment();
        // shift = 0.f;
      }
    }

    setSegment(segment, shift);
    m_bogie_pos_forward =
        calcBackBogiePos(m_bogie_pos_back, m_bogie_pos_forward);
  }
}

TrainController::BogiePos TrainController::calcNextPos(BogiePos pos,
                                                       float ifps) {
  BogiePos ret_pos = pos;
}

TrainController::BogiePos TrainController::calcBackBogiePos(BogiePos forward,
                                                            BogiePos back) {
  BogiePos ret_pos = back;

  Math::Vec3 v_forward =
      forward.m_curr_segment->calcPoint(forward.m_t_coordinate);
  Math::Vec3 v_back = back.m_curr_segment->calcPoint(back.m_t_coordinate);
  float distance = (v_forward - v_back).length();
  float excess = distance - m_bogie_distance;
  // m_current_move_direction == MOVE_DIRECTION::FORWARD ? 1.f : -1.f;
  float new_t_coordinate = ret_pos.m_t_coordinate;

  int count = 0;

  while (Math::abs(excess) > Math::Consts::EPS && count < 10) {
    // if(excess > Math::Consts::EPS)
    if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
      new_t_coordinate += excess / ret_pos.m_curr_segment_len;
    else
      new_t_coordinate -= excess / ret_pos.m_curr_segment_len;

    while (new_t_coordinate > 1.f) {
      ret_pos.m_curr_segment = m_callback_next_segment_f(
          ret_pos.m_curr_segment);  // m_prev_segment;
                                    // //m_bogie_pos_forward.m_curr_segment;

      if (!ret_pos.m_curr_segment) {
        ret_pos.m_curr_segment = getCurrentSegment();
        new_t_coordinate = 1.f;
      } else
        new_t_coordinate = (new_t_coordinate - 1.f) *
                           ret_pos.m_curr_segment_len /
                           ret_pos.m_curr_segment->getLength();

      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    }
    while (new_t_coordinate < 0.f) {
      ret_pos.m_curr_segment = m_callback_prev_segment_f(
          ret_pos.m_curr_segment);  // m_prev_segment ;//
                                    // m_bogie_pos_back.m_curr_segment;

      if (!ret_pos.m_curr_segment) {
        ret_pos.m_curr_segment = getCurrentSegment();
        new_t_coordinate = 0.f;
      } else
        new_t_coordinate = 1.f + new_t_coordinate * ret_pos.m_curr_segment_len /
                                     ret_pos.m_curr_segment->getLength();

      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    }

    v_back = ret_pos.m_curr_segment->calcPoint(new_t_coordinate);

    distance = (v_forward - v_back).length();

    excess = distance - m_bogie_distance;

    ++count;
  }

  m_count_collection.push_back(count);

  // Log::message("%d ", count);

  ret_pos.m_t_coordinate = new_t_coordinate;

  return ret_pos;
}

void TrainController::setSegment(Unigine::SplineSegmentPtr curr_segment,
                                 float pos) {
  if (!curr_segment) return;

  if (!m_prev_segment) {
    if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
      m_prev_segment = m_callback_prev_segment_f(curr_segment);
    else
      m_prev_segment = m_callback_next_segment_f(curr_segment);
  }

  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    setSegmentForward(curr_segment, pos);
  else
    setSegmentReverse(curr_segment, pos);
}

void TrainController::setSegmentForward(Unigine::SplineSegmentPtr curr_segment,
                                        float pos) {
  BogiePos& first_bogie = m_bogie_pos_forward;
  BogiePos& second_bogie = m_bogie_pos_back;

  first_bogie.m_curr_segment = curr_segment;

  if (Math::abs(m_bogie_distance) < Math::Consts::EPS) return;

  Unigine::SplineSegmentPtr tmp_segment;

  float len = curr_segment->getLength();

  // if (first_bogie.m_curr_segment != curr_segment) m_curr_segment =
  // curr_segment;

  first_bogie.m_t_coordinate = Math::clamp(pos, 0.f, 1.f);
  first_bogie.m_curr_segment_len = len;

  if (first_bogie.m_t_coordinate * len < m_bogie_distance) {
    tmp_segment = m_prev_segment;  // m_callback_prev_segment_f(curr_segment);

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
                                        float pos) {
  BogiePos& first_bogie = m_bogie_pos_back;
  BogiePos& second_bogie = m_bogie_pos_forward;

  first_bogie.m_curr_segment = curr_segment;

  if (Math::abs(m_bogie_distance) < Math::Consts::EPS) return;

  Unigine::SplineSegmentPtr tmp_segment;

  float len = curr_segment->getLength();

  // if (first_bogie.m_curr_segment != curr_segment) m_curr_segment =
  // curr_segment;

  first_bogie.m_t_coordinate = Math::clamp(pos, 0.f, 1.f);
  first_bogie.m_curr_segment_len = len;

  if ((1.f - first_bogie.m_t_coordinate) * len < m_bogie_distance) {
    tmp_segment = m_prev_segment;  // m_callback_next_segment_f(curr_segment);

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