#pragma once

#include <UnigineComponentSystem.h>

#include <memory>

#include "Carriage.h"
#include "InputController.h"
#include "MovementLogic.h"
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

  void init();
  void update();

 private:
  Unigine::PlayerSpectatorPtr m_head_camera;
  Unigine::PlayerSpectatorPtr m_tail_camera;
  Unigine::PlayerSpectatorPtr m_world_camera;
};