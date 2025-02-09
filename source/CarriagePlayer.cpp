#include "CarriagePlayer.h"

REGISTER_COMPONENT(CarriagePlayer);

using namespace Unigine;

void CarriagePlayer::init() {
  Vector<NodePtr> root_nodes;
  World::getNodes(root_nodes);

  TrainManager::setCarriage(this);
  setTrains();

  setSplineSegment(TrainManager::getStartSegment());
  // TrainManager* p_train_manager = nullptr;
  // for (auto& it : root_nodes) {
  //   if (p_train_manager =
  //           ComponentSystem::get()->getComponentInChildren<TrainManager>(it))
  //           {
  //     //p_train_manager->setCarriage(this);

  //     break;
  //   }
  // }

  if (m_trains.size()) {
    m_trains[0]->setNextSegmentFunction(  // std::bind(
        &TrainManager::
            getNextSegment /*, p_train_manager, std::placeholders::_1)*/);

    Log::message("Carriage Player: %d trains are load\n", m_trains.size());
  }

  m_head_camera = checked_ptr_cast<PlayerPersecutor>(head_camera.get());
  m_tail_camera = checked_ptr_cast<PlayerPersecutor>(tail_camera.get());
  m_world_camera = checked_ptr_cast<PlayerSpectator>(world_camera.get());

  if (m_head_camera) {
    m_head_camera->setMainPlayer(true);
    Log::message("Carriage Player: head camera is load\n", "\n");
  }

  if (m_tail_camera)
    Log::message("Carriage Player: tail camera is load\n", "\n");
  if (m_world_camera)
    Log::message("Carriage Player: world camera is load\n", "\n");
}

void CarriagePlayer::update() {
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
    m_world_camera->setMainPlayer(true);

  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::LOCAL_CAMERA_SWITCH))
    is_local_camera_switch = true;

  if (is_local_camera_switch &&
      m_trains[0]->getMoveDirection() == Train::MOVE_DIRECTION::FORWARD)
    m_head_camera->setMainPlayer(true);

  if (is_local_camera_switch &&
      m_trains[0]->getMoveDirection() == Train::MOVE_DIRECTION::REVERSE)
    m_tail_camera->setMainPlayer(true);
}