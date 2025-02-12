#include "CarriagePlayer.h"

REGISTER_COMPONENT(CarriagePlayer);

using namespace Unigine;

void CarriagePlayer::init() {
  Vector<NodePtr> root_nodes;
  World::getNodes(root_nodes);

  TrainManager::setCarriage(this);
  setTrains();

  int count_ready = 0;

  // do
  // {
  //   count_ready = 0;
  //   for(auto it : m_trains) {
  //     if(it->isInitialized())
  //       ++count_ready;
  //   }
  // } while (!count_ready);

  // setSplineSegment(TrainManager::getStartSegment());

  if (m_trains.size()) {
    m_trains[0]->setPrevSegmentFunction(  // std::bind(
        &TrainManager::
            getPrevSegment /*, p_train_manager, std::placeholders::_1)*/);

    m_trains[0]->setNextSegmentFunction(&TrainManager::getNextSegment);

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
  if (!m_trains[0]->getCurrentSegment()) setStartPos();

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

  if (is_local_camera_switch && m_trains[0]->getMoveDirection() ==
                                    TrainController::MOVE_DIRECTION::FORWARD)
    m_head_camera->setMainPlayer(true);

  if (is_local_camera_switch && m_trains[0]->getMoveDirection() ==
                                    TrainController::MOVE_DIRECTION::REVERSE)
    m_tail_camera->setMainPlayer(true);

  // setPos();
}

void CarriagePlayer::setStartPos() {
  if (m_trains.empty()) return;

  SplineSegmentPtr current_segment = TrainManager::getStartSegment();

  int len = current_segment->getLength();
  float pos = 0.f;

  for (auto it = m_trains.begin(), it_end = m_trains.end(); it != it_end;
       ++it) {
    if (!current_segment) break;

    // it->getNode()->setWorldPosition(Math::Vec3_one);
    (*it)->setSegment(current_segment, pos);

    if (Math::abs(len) < Math::Consts::EPS) continue;

    pos -= ((*it)->getLength() + 2.f) / len;
    if (pos < 0.f) {
      current_segment = TrainManager::getPrevSegment(current_segment);

      pos = 1.f - (-pos) * len / current_segment->getLength();
      len = current_segment->getLength();
    }
  }
}

void CarriagePlayer::setPos() {
  SplineSegmentPtr current_segment = m_trains[0]->getCurrentSegment();

  if (!current_segment) return;

  int len = current_segment->getLength();
  float pos = 0.f;

  for (auto it = m_trains.begin(), it_end = m_trains.end(); it != it_end;
       ++it) {
    if (!current_segment) break;

    // it->getNode()->setWorldPosition(Math::Vec3_one);
    (*it)->setSegment(current_segment, pos);

    if (Math::abs(len) < Math::Consts::EPS) continue;

    pos -= (*it)->getLength() / len;
    if (pos < 0.f) {
      current_segment = TrainManager::getPrevSegment(current_segment);

      pos = 1.f - (-pos) * len / current_segment->getLength();
      len = current_segment->getLength();
    }
  }
}