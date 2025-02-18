#include "Carriage.h"

using namespace Unigine;

REGISTER_COMPONENT(Carriage);

using namespace Unigine;

void Carriage::init() {}

void Carriage::setTrains() {
  NodePtr node = getNode();

  getComponentsInChildren<TrainController>(getNode(), m_trains);

  for (auto train : m_trains) {
    train->setMoveStartFunc(std::bind(&Carriage::startMove, this));
    train->setMoveEndFunc(std::bind(&Carriage::stopMove, this));
  }
}

// void Carriage::setSplineSegment(SplineSegmentPtr segment) {
//   for (auto train : m_trains) {
//     train->setTrainAtSegment(segment);
//     train->setMoveStartFunc(std::bind(&Carriage::startMove, this));
//     train->setMoveEndFunc(std::bind(&Carriage::stopMove, this));
//   }
// }
