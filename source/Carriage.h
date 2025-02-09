#pragma once

#include <UnigineComponentSystem.h>

#include "TrainController.h"

class Carriage : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(Carriage, Unigine::ComponentBase);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  void setSplineSegment(Unigine::SplineSegmentPtr segment);

 protected:
  void init();
  void update() {}

  void setTrains();

 protected:
  // Контейнер вагонов. 0 - голова, N - хвост.
  Unigine::Vector<TrainController*> m_trains;
};
