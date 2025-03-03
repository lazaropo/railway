#include "Train.h"

REGISTER_COMPONENT(Train);

using namespace Unigine;

void Train::init() {
  ComponentSystem::get()->getComponentsInChildren<Carriage>(getNode(),
                                                            m_carriage);

  if (m_carriage.size()) {
    m_carraige_len = m_carriage[0]->getNode()->getBoundBox().getSize().y;
  }
}

void Train::update() {
  if (m_is_stop || m_position.isEmpty()) return;

  m_carraige_len = m_carriage[0]->getLength();

  float shift = Math::min(m_speed, max_speed) * Game::getIFps();
  float distance = shift;

  auto tmp_carriage = m_move_direction == Carriage::MOVE_DIRECTION::FORWARD
                          ? m_carriage.begin()
                          : --m_carriage.end();
  SegmentPosition tmp_segm_pos;

  tmp_segm_pos = (*tmp_carriage)->getSegmentPosition(m_move_direction);
  if (tmp_segm_pos.isEmpty()) {
    (*tmp_carriage)->setPosition(m_position, m_move_direction);
    tmp_segm_pos = m_position;
  }

  if (m_move_direction == Carriage::MOVE_DIRECTION::FORWARD) {
    m_position = tmp_segm_pos.moveBy(distance);
    distance -= margin + m_carraige_len;
  } else {
    distance = -distance;
    m_position = tmp_segm_pos.moveBy(distance);
    distance += margin + m_carraige_len;
  }

  tmp_segm_pos = m_position;

  // distance += m_move_direction == Carriage::MOVE_DIRECTION::FORWARD
  //                 ? (-margin - m_carraige_len)
  //                 : (margin + m_carraige_len);

  if (tmp_segm_pos.isEmpty()) {
    m_is_stop = true;
    return;
  }

  if (m_move_direction == Carriage::MOVE_DIRECTION::FORWARD) {
    for (auto it = m_carriage.begin(), it_end = m_carriage.end(); it != it_end;
         ++it) {
      (*it)->setPosition(tmp_segm_pos, m_move_direction);
      tmp_segm_pos = tmp_segm_pos.calcByDistance(distance);
    }
  } else {
    for (auto it = m_carriage.rbegin(), it_end = m_carriage.rend();
         it != it_end; ++it) {
      (*it)->setPosition(tmp_segm_pos, m_move_direction);
      tmp_segm_pos = tmp_segm_pos.calcByDistance(distance);
    }
  }
}

void Train::setStartPosition(Unigine::SplineSegmentPtr segment,
                             float t_coordinate) {
  SegmentPosition segm_pos(segment, t_coordinate);

  m_position = segm_pos;
}
