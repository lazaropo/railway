#pragma once

#include <UnigineComponentSystem.h>

#include "Carriage.h"

class CarriageController : public Carriage {
 public:
  COMPONENT_DEFINE(CarriageController, Carriage);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

 private:
  Unigine::PlayerPersecutorPtr m_head_camera;
  Unigine::PlayerPersecutorPtr m_tail_camera;
  Unigine::PlayerSpectatorPtr m_world_camera;

 protected:
  void init();
  void update();
};