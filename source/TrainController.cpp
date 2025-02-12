#include "TrainController.h"

REGISTER_COMPONENT(TrainController);

using namespace Unigine;

void TrainController::init() {
  // Vector<Train*> trains;
  // ComponentSystem::get()->getComponentsInChildren<Train>(getNode(), trains);
  NodePtr train_node = getNode();
  // if(getNode()->findChild("train_locomotive.node") >= 0)
  //   train_node= World::loadNode("train_locomotive.node");
  // else
  //   train_node = World::loadNode("train_tanker.node");

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

  if (Math::abs(m_bogie_distance) < Math::Consts::EPS && m_forward_bogie &&
      m_back_bogie)
    m_bogie_distance =
        (m_forward_bogie->getPosition() - m_back_bogie->getPosition()).length();

  // float ifps = Game::getIFps();

  moveTrain();

  if (!m_curr_segment) return;

  Math::Vec3 forward_pos = m_bogie_pos_forward.m_curr_segment->calcPoint(
      m_bogie_pos_forward.m_t_coordinate);
  Math::Vec3 back_pos = m_bogie_pos_back.m_curr_segment->calcPoint(
      m_bogie_pos_back.m_t_coordinate);

  m_forward_bogie->setPosition(forward_pos);
  m_forward_bogie->setDirection(m_bogie_pos_forward.m_curr_segment->calcTangent(
                                    m_bogie_pos_forward.m_t_coordinate),
                                Math::vec3_up, Math::AXIS_Y);

  m_car->setPosition(
      (forward_pos - back_pos) / 2 + back_pos
      // m_car_pos.m_curr_segment->calcPoint(m_car_pos.m_t_coordinate)
  );
  m_car->setDirection(
      // m_car_pos.m_curr_segment->calcTangent(m_car_pos.m_t_coordinate),
      Math::vec3(m_forward_bogie->getPosition() - m_back_bogie->getPosition()),
      Math::vec3_up, Math::AXIS_Y);

  m_back_bogie->setPosition(back_pos);
  m_back_bogie->setDirection(m_bogie_pos_back.m_curr_segment->calcTangent(
                                 m_bogie_pos_back.m_t_coordinate),
                             // Math::vec3(m_forward_bogie->getPosition() -
                             // m_back_bogie->getPosition()),
                             Math::vec3_up, Math::AXIS_Y);

  auto pos = m_car->getPosition();
  m_forward_bogie->renderBounds();
  m_car->renderBounds();

  renderNode(m_car);
  renderNode(m_forward_bogie);

  // Visualizer::renderVector(pos, pos +
  // (Math::Vec3)m_car->getDirection(Math::AXIS_Y),
  //                          Math::vec4_black);
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

  m_current_linear_velocity = start_speed;
}

void TrainController::moveTrain() {
  if (!m_forward_bogie || !m_back_bogie || !m_car || !m_callback_prev_segment_f)
    return;

  float ifps = Game::getIFps();

  m_current_linear_velocity = takeNext(
      m_current_linear_velocity, m_new_linear_velocity, start_speed * ifps);
  m_current_linear_velocity =
      Math::clamp(m_current_linear_velocity, 0.f, max_speed);

  if (m_current_move_direction == MOVE_DIRECTION::FORWARD) {
    float shift = m_bogie_pos_forward.m_t_coordinate +
                  m_current_linear_velocity * ifps /
                      m_bogie_pos_forward.m_curr_segment_len;
    SplineSegmentPtr segment = m_curr_segment;
    if (shift >= 1.f) {
      segment = m_callback_next_segment_f(m_curr_segment);
      shift = (shift - 1.f) * m_bogie_pos_forward.m_curr_segment_len /
              segment->getLength();
    }

    setSegment(segment, shift);
    m_bogie_pos_back = calcBackBogiePos(m_bogie_pos_forward);
  } else {
    float shift =
        m_bogie_pos_back.m_t_coordinate -
        m_current_linear_velocity * ifps / m_bogie_pos_back.m_curr_segment_len;
    SplineSegmentPtr segment = m_curr_segment;
    if (shift < 0.f) {
      segment = m_callback_prev_segment_f(m_curr_segment);
      shift = (shift + 1.f) * m_bogie_pos_back.m_curr_segment_len /
              segment->getLength();
    }

    setSegment(segment, shift);
    m_bogie_pos_back = calcBackBogiePos(m_bogie_pos_forward);
  }

  // if (m_bogie_pos_forward.m_curr_segment)
  //   m_bogie_pos_forward = calcNextPos(m_bogie_pos_forward, ifps);
  // if (m_bogie_pos_back.m_curr_segment)
  //   m_bogie_pos_back = calcNextPos(m_bogie_pos_back, ifps);
  // if (m_car_pos.m_curr_segment) m_car_pos = calcNextPos(m_car_pos, ifps);

  // Math::Mat4 transform =
  // makeBogieTransform(m_forward_bogie, m_bogie_pos_forward, ifps);

  // setSegment(m_bogie_pos_forward.m_curr_segment,
  //            m_bogie_pos_forward.m_t_coordinate + m_current_linear_velocity /
  //            m_bogie_pos_forward.m_curr_segment_len);

  // transform = makeBogieTransform(back_bogie, m_bogie_pos_back, ifps);
  // //if (transform != Math::Mat4_zero) back_bogie->setTransform(transform);
  // transform = makeBogieTransform(car_body, m_car_pos, ifps);

  // setCarBody(car_body);
}

void TrainController::setCarBody(NodePtr body) {
  if (!body) return;

  // Math::Mat4 transform = body->getTransform();

  // Math::Vec3 forward_bogie_pos = forward_bogie->getPosition();
  // Math::Vec3 back_bogie_pos = back_bogie->getPosition();

  // Math::Vec3 pos = (back_bogie_pos + back_bogie_pos) / 2.f;

  // body->setPosition(pos);

  // forward_bogie->getWorldRotation();
}

TrainController::BogiePos TrainController::calcNextPos(BogiePos pos,
                                                       float ifps) {
  BogiePos ret_pos = pos;

  // float relative_velocity =
  //     m_current_linear_velocity * ifps / ret_pos.m_curr_segment_len;

  // while (m_current_move_direction == MOVE_DIRECTION::FORWARD &&
  //        (ret_pos.m_t_coordinate + relative_velocity >= 1.f)) {
  //   if (!ret_pos.m_curr_segment) return ret_pos;

  //   ret_pos.m_curr_segment =
  //   m_callback_next_segment_f(ret_pos.m_curr_segment); ret_pos.m_t_coordinate
  //   += relative_velocity - 1.f; ret_pos.m_t_coordinate =
  //   ret_pos.m_t_coordinate *
  //                            ret_pos.m_curr_segment_len /
  //                            ret_pos.m_curr_segment->getLength();
  //   ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();

  //   relative_velocity =
  //       m_current_linear_velocity * ifps / ret_pos.m_curr_segment_len;
  // }

  // while (m_current_move_direction == MOVE_DIRECTION::REVERSE &&
  //        (ret_pos.m_t_coordinate - relative_velocity <= 0.f)) {
  //   if (!ret_pos.m_curr_segment) return ret_pos;

  //   ret_pos.m_t_coordinate -= relative_velocity;
  //   ret_pos.m_t_coordinate = 1.f - (-ret_pos.m_t_coordinate) *
  //                                      ret_pos.m_curr_segment_len /
  //                                      ret_pos.m_curr_segment->getLength();
  //   ret_pos.m_curr_segment =
  //   m_callback_next_segment_f(ret_pos.m_curr_segment);
  //   ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();

  //   relative_velocity =
  //       m_current_linear_velocity * ifps / ret_pos.m_curr_segment_len;
  // }

  // if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
  //   ret_pos.m_t_coordinate += relative_velocity;
  // else
  //   ret_pos.m_t_coordinate -= relative_velocity;

  // return ret_pos;
}

TrainController::BogiePos TrainController::calcBackBogiePos(BogiePos forward) {
  BogiePos ret_pos = forward;

  Math::Vec3 v_forward =
      forward.m_curr_segment->calcPoint(forward.m_t_coordinate);
  Math::Vec3 v_back = v_forward;
  float distance = 0;
  float excess = 1;
  float new_t_coordinate = ret_pos.m_t_coordinate;

  int count = 0;

  while (Math::abs(excess) > Math::Consts::EPS && count < 10) {
    // if(excess > Math::Consts::EPS)
    new_t_coordinate += excess / ret_pos.m_curr_segment_len;
    if (new_t_coordinate > 1.f) {
      ret_pos.m_curr_segment =
          m_callback_next_segment_f(ret_pos.m_curr_segment);

      new_t_coordinate = (new_t_coordinate - 1.f) * ret_pos.m_curr_segment_len /
                         ret_pos.m_curr_segment->getLength();

      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    } else if (new_t_coordinate < 0.f) {
      ret_pos.m_curr_segment =
          m_callback_prev_segment_f(ret_pos.m_curr_segment);

      new_t_coordinate = 1.f - (-new_t_coordinate) *
                                   ret_pos.m_curr_segment_len /
                                   ret_pos.m_curr_segment->getLength();

      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    }

    v_back = ret_pos.m_curr_segment->calcPoint(new_t_coordinate);

    distance = (v_forward - v_back).length();
    excess = distance - m_bogie_distance;

    ++count;
  }

  m_count_collection.push_back(count);

  Log::message("%d ", count);

  ret_pos.m_t_coordinate = new_t_coordinate;

  return ret_pos;
}

Math::Mat4 TrainController::makeBogieTransform(const NodePtr node, BogiePos pos,
                                               float ifps) {
  if (!node) return Math::Mat4_zero;

  Math::Mat4 transform = node->getTransform();
  Math::vec3 direction = node->getDirection();

  float relative_velocity =
      m_current_linear_velocity * ifps / pos.m_curr_segment_len;

  while (m_current_move_direction == MOVE_DIRECTION::FORWARD &&
         (pos.m_t_coordinate + relative_velocity >= 1.f)) {
    if (!pos.m_curr_segment) return Math::Mat4_zero;

    pos.m_t_coordinate -= 1.f;
    pos.m_curr_segment = m_callback_prev_segment_f(pos.m_curr_segment);
    pos.m_curr_segment_len = pos.m_curr_segment->getLength();
  }

  while (m_current_move_direction == MOVE_DIRECTION::REVERSE &&
         (pos.m_t_coordinate - relative_velocity <= 0.f)) {
    if (!pos.m_curr_segment) return Math::Mat4_zero;

    pos.m_t_coordinate += 1.f;
    pos.m_curr_segment = m_callback_prev_segment_f(pos.m_curr_segment);
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

  transform.setRotateZ(Math::sign(
      Math::dot(Math::vec3_up, Math::cross(direction, new_direction)) *
      Math::getAngle(direction, new_direction)));
  transform.setTranslate(new_pos - transform.getColumn3(3));

  Visualizer::renderDirection(new_pos, new_direction, Math::vec4_blue);

  return transform;
}

void TrainController::setSegment(Unigine::SplineSegmentPtr curr_segment,
                                 float pos) {
  if (!curr_segment || Math::abs(m_bogie_distance) < Math::Consts::EPS) {
    if (curr_segment) {
      m_bogie_pos_forward.m_curr_segment = curr_segment;
      m_curr_segment = curr_segment;
    }
    return;
  }

  Unigine::SplineSegmentPtr prev_segment;

  float len = curr_segment->getLength();

  if (m_bogie_pos_forward.m_curr_segment != curr_segment)
    m_curr_segment = curr_segment;

  m_bogie_pos_forward.m_curr_segment = curr_segment;
  m_bogie_pos_forward.m_t_coordinate = Math::clamp(pos, 0.f, 1.f);
  m_bogie_pos_forward.m_curr_segment_len = len;

  if (m_bogie_pos_forward.m_t_coordinate * len < m_bogie_distance) {
    prev_segment = m_callback_prev_segment_f(curr_segment);

    m_bogie_pos_back.m_curr_segment = prev_segment;
    m_bogie_pos_back.m_curr_segment_len = prev_segment->getLength();
    float back_t_pos =
        1.f - (m_bogie_distance - m_bogie_pos_forward.m_t_coordinate * len) /
                  m_bogie_pos_back.m_curr_segment_len;
    m_bogie_pos_back.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);

  } else {
    m_bogie_pos_back.m_curr_segment = curr_segment;
    m_bogie_pos_back.m_curr_segment_len = len;
    float back_t_pos =
        m_bogie_pos_forward.m_t_coordinate - m_bogie_distance / len;
    m_bogie_pos_back.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);
  }

  if (m_bogie_pos_forward.m_t_coordinate * len < m_bogie_distance / 2.) {
    m_car_pos.m_curr_segment = prev_segment;
    m_car_pos.m_curr_segment_len = prev_segment->getLength();
    len = m_car_pos.m_curr_segment_len;

    float car_t_pos =
        m_bogie_pos_back.m_t_coordinate + 0.5 * m_bogie_distance / len;
    m_car_pos.m_t_coordinate = Math::clamp(car_t_pos, 0.f, 1.f);

  } else {
    m_car_pos.m_curr_segment = curr_segment;
    m_car_pos.m_curr_segment_len = len;
    float car_t_pos =
        m_bogie_pos_forward.m_t_coordinate - 0.5 * m_bogie_distance / len;
    m_car_pos.m_t_coordinate = Math::clamp(car_t_pos, 0.f, 1.f);
  }

  // Visualizer::renderVector(position, (Math::Vec3)direction,
  // Math::vec4_white);
}
