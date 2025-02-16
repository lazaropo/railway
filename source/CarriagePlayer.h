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
  // void setPos() override;

  void init();
  void update();

  COMPONENT_SHUTDOWN(shutdown);
  // COMPONENT_UPDATE(update);
  void shutdown() {
    int count = 0;

    for (auto current : m_trains) {
      auto prev = TrainManager::getPrevSegment(current->getCurrentSegment());
      Unigine::SplineSegmentPtr prev_next = nullptr;
      if (prev) prev_next = TrainManager::getNextSegment(prev);

      if (prev_next != current->getCurrentSegment()) count++;
    }
  }

 private:
  Unigine::PlayerPersecutorPtr m_head_camera;
  Unigine::PlayerPersecutorPtr m_tail_camera;
  Unigine::PlayerSpectatorPtr m_world_camera;
};