#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

#include "TrainController.h"

class Carriage : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(Carriage, Unigine::ComponentBase);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  //  void setSplineSegment(Unigine::SplineSegmentPtr segment);

  void stopMove() {
    for (auto train : m_trains) train->stopMove();
  }

  void startMove() {
    for (auto train : m_trains) train->startMove();
  }

  float getIfps() {
    if (m_count++ == m_trains.size()) {
      m_count = 0;
      m_ifps = Unigine::Game::getIFps();
    }
    return m_ifps;
  }

 protected:
  void init();
  void update() {}

  void setTrains();

  virtual void setStartPos() {}
  virtual void setPos() {}

 protected:
  // Контейнер вагонов. 0 - голова, N - хвост.
  Unigine::Vector<TrainController*> m_trains;

  float m_ifps = 0.f;
  size_t m_count = m_trains.size();
};
