#pragma once

#include <UnigineComponentSystem.h>

#include "TrainController.h"

class Carriage : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(Carriage, Unigine::ComponentBase);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  PROP_PARAM(Node, head_camera);
  PROP_PARAM(Node, tail_camera);
  PROP_PARAM(Node, world_camera);

 protected:
  void init();
  void update();

 protected:
  // Контейнер вагонов. 0 - голова, N - хвост.
  Unigine::Vector<TrainController*> m_trains;
};
