#pragma once

#include <UnigineComponentSystem.h>

#include "Train.h"
#include "TrainManager.h"

class TrainController : public Train {
 public:
  COMPONENT_DEFINE(TrainController, Train);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  void moveTrain() override;

 private:
  void moveBogie(NodePtr bogie);
  void setCarBody(NodePtr body);

  Unigine::Math::Mat4 getBogieTransform();

 protected:
  void init();
  void update();

 private:
  TrainManager* m_train_manager = nullptr;

  Unigine::PlayerPersecutorPtr m_head_camera;
  Unigine::PlayerPersecutorPtr m_tail_camera;
  Unigine::PlayerSpectatorPtr m_world_camera;
};
