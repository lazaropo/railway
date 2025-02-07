#include "TrainController.h"

REGISTER_COMPONENT(TrainController);

using namespace Unigine;

void TrainController::update() {
  if (!m_curr_segment) return;

  moveBogie(forward_bogey);
  moveBogie(back_bogey);

  setCarBody(car_body);

  Train::update();
}

void TrainController::moveBogie(NodePtr bogie) {
  bogie->setWorldTransform(getBogieTransform());
}

void TrainController::setCarBody(NodePtr body) {}

Math::Mat4 TrainController::getBogieTransform() {}
