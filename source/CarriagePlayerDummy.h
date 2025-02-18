#pragma once

#include <UnigineComponentSystem.h>

#include <memory>

#include "Carriage.h"
#include "MovementLogic.h"
#include "TrainManager.h"

class CarriagePlayerDummy : public Carriage {
 public:
  COMPONENT_DEFINE(CarriagePlayerDummy, Carriage);

  PROP_PARAM(Float, const_move_speed, 20.f);

  COMPONENT_INIT(init);

 protected:
  void init();
};