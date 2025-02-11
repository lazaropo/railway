#pragma once

#include <UnigineComponentSystem.h>

#include "Carriage.h"
#include "TrainManager.h"

class CarriagePlayer : public Carriage {
 public:
  COMPONENT_DEFINE(CarriagePlayer, Carriage);

  PROP_PARAM(Node, head_camera);
  PROP_PARAM(Node, tail_camera);
  PROP_PARAM(Node, world_camera);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

 protected:
  void setStartPos() override;
  void setPos() override;

  void init();
  void update();

 private:
  Unigine::PlayerPersecutorPtr m_head_camera;
  Unigine::PlayerPersecutorPtr m_tail_camera;
  Unigine::PlayerSpectatorPtr m_world_camera;
};