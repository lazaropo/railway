#include "Train.h"

REGISTER_COMPONENT(Train);

using namespace Unigine;

void Train::init() {
  //   Vector<Carriage*> carriages;
  //   ComponentSystem::get()->getComponents<Carriage*>(getNode(), carriages);
  ComponentSystem::get()->getComponentsInChildren<Carriage>(getNode(),
                                                            m_carriage);
  if (m_carriage.size()) {
    m_carraige_len = m_carriage[0]->getNode()->getBoundBox().getSize().y;
  }
  //   for (auto it : carriages)
  //     m_carriage.push_back(std::move(std::unique_ptr(it)));
}

void Train::update() {
  if (m_is_stop || m_position.isEmpty()) return;

  if (!m_carraige_len) {
    ObjectMeshStaticPtr mesh =
        checked_ptr_cast<ObjectMeshStatic>(m_carriage[0]->getNode());
    if (mesh) m_carraige_len = mesh->getBoundBox().getSize().y;
  }
  m_carraige_len = m_carriage[0]->getLength();

  float shift = Math::min(m_speed, max_speed) * Game::getIFps();
  float distance = shift;

  m_position = m_position.moveBy(distance);
  distance -= margin + m_carraige_len;

  SegmentPosition segm_pos = m_position;
  if (segm_pos.isEmpty()) {
    m_is_stop = true;
    return;
  }

  for (auto it : m_carriage) {
    //   if (segm_pos.isEmpty()) {
    //   m_is_stop = true;
    //   break;
    // }

    it->setPosition(segm_pos);

    segm_pos = segm_pos.calcByDistance(distance);
  }
}

void Train::setStartPosition(Unigine::SplineSegmentPtr segment,
                             float t_coordinate) {
  SegmentPosition segm_pos(segment, t_coordinate);

  m_position = segm_pos;

  //   float shift = m_carriage[0]->getNode()->getBoundBox().getSize().y;
  //   float distance = 0;

  //   for (auto it : m_carriage) {
  //     it->setPosition(segm_pos);
  //     // it->setStartPosition(distance + margin);
  //     segm_pos = segm_pos.calcByDistance(distance + margin);
  //     distance += shift;
  //   }
}

// void Train::moveBy() {}