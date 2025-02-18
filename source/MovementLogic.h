#pragma once

#include "IMovementLogic.h"
#include "TrainController.h"

class MovementLogic : public IMovementLogic {
 public:
  BogiePos moveFirstBogie(BogiePos first, float v_speed) override;
  BogiePos calcSecondBogiePos(const BogiePos& first, BogiePos second,
                              float distance_btw_bogie) override;
};
