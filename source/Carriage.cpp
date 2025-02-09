#include "Carriage.h"

using namespace Unigine;

REGISTER_COMPONENT(Carriage);

using namespace Unigine;

void Carriage::init() {
  //   NodePtr node = getNode();
  //   for (int i = 0, i_end = node->getNumChildren(); i < i_end; ++i)
  //     if (TrainController* train =
  //             ComponentSystem::get()->getComponentInChildren<TrainController>(
  //                 node->getChild(i)))
  //       m_trains.push_back(train);
}

void Carriage::setTrains() {
  NodePtr node = getNode();

  //   for (int i = 0, i_end = node->getNumChildren(); i < i_end; ++i)
  //     if (TrainController* train =
  //             ComponentSystem::get()->getComponentsInChildren<TrainController>(
  //                 node->getChild(i)))
  //       m_trains.push_back(train);

  getComponentsInChildren<TrainController>(getNode(), m_trains);
}

void Carriage::setSplineSegment(SplineSegmentPtr segment) {
  for (auto it : m_trains) it->setSegment(segment);
}
