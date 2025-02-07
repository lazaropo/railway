#include "CarriageController.h"

REGISTER_COMPONENT(CarriageController);

using namespace Unigine;

void CarriageController::init() {
  Vector<NodePtr> root_nodes;
  World::getRootNodes(root_nodes);
  for (auto& it : root_nodes)
    if (TrainManager* ptr =
            ComponentSystem::get()->getComponentInChildren<TrainManager>(it))
      ptr->setTrain(this);

  m_head_camera = checked_ptr_cast<PlayerPersecutor>(head_camera->getNode(0));
  m_tail_camera = checked_ptr_cast<PlayerPersecutor>(tail_camera->getNode(0));
  m_world_camera = checked_ptr_cast<PlayerSpectator>(world_camera->getNode(0));
}

void CarriageController::update() {
  float linear_acceleration = 0;
  float tmp = 0;

  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_DECREASE);
  linear_acceleration -= Math::clamp(tmp, 0.f, 1.f);

  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_INCREASE);
  linear_acceleration += Math::clamp(tmp, 0.f, 1.f);

  // for (auto it : m_trains) it->setAcceleration(linear_acceleration);
  bool is_local_camera_switch = false;

  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::TURN_AROUND)) {
    for (auto it : m_trains) {
      it->setAcceleration(linear_acceleration);
      it->changeMoveDirection();
    }
    is_local_camera_switch = true;
  } else
    for (auto it : m_trains) it->setAcceleration(linear_acceleration);

  // Нужно ли здесь отключать другие камеры?
  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::WORLD_CAMERA_SWITCH))
    world_camera->setEnabled(true);

  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::LOCAL_CAMERA_SWITCH))
    is_local_camera_switch = true;

  if (is_local_camera_switch &&
      m_trains[0]->getMoveDirection() == Train::MOVE_DIRECTION::FORWARD)
    head_camera->setEnabled(true);

  if (is_local_camera_switch &&
      m_trains[0]->getMoveDirection() == Train::MOVE_DIRECTION::REVERSE)
    tail_camera->setEnabled(true);
}