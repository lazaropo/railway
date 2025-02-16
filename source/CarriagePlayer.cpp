#include "CarriagePlayer.h"

REGISTER_COMPONENT(CarriagePlayer);

using namespace Unigine;

void CarriagePlayer::init() {
  Vector<NodePtr> root_nodes;
  World::getNodes(root_nodes);

  TrainManager::setCarriage(this);
  setTrains();

  int count_ready = 0;

  if (m_trains.size()) {
    m_trains[0]->setPrevSegmentFunction(&TrainManager::getPrevSegment);

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

  // if (m_tail_camera){
  //   m_head_camera->translate((*m_trains.front())->getNode()->getPosition());
  //   Log::message("Carriage Player: tail camera is load\n", "\n");}
  // if (m_world_camera){
  //   m_tail_camera->translate((*m_trains.back())->getNode()->getPosition());
  //   Log::message("Carriage Player: world camera is load\n", "\n");}
}

void CarriagePlayer::update() {
  if (!m_trains[0]->getCurrentSegment()) setStartPos();

  float linear_acceleration = 0;
  float tmp = 0;

  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_DECREASE);
  for (auto it : m_trains) it->brake();

  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_INCREASE);
  for (auto it : m_trains) it->accelerate();

  // for (auto it : m_trains) it->setAcceleration(linear_acceleration);
  bool is_local_camera_switch = false;

  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::TURN_AROUND)) {
    for (auto train : m_trains) {
      train->changeMoveDirection();
      train->startMove();
    }
    is_local_camera_switch = true;
  }
  // } else
  //   for (auto it : m_trains) it->setAcceleration(linear_acceleration);

  // Нужно ли здесь отключать другие камеры?
  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::WORLD_CAMERA_SWITCH)) {
    m_world_camera->setMainPlayer(true);
    m_head_camera->setMainPlayer(false);
    m_tail_camera->setMainPlayer(false);
  }

  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::LOCAL_CAMERA_SWITCH))
    is_local_camera_switch = true;

  if (is_local_camera_switch && m_trains[0]->getMoveDirection() ==
                                    TrainController::MOVE_DIRECTION::FORWARD) {
    m_world_camera->setMainPlayer(false);
    m_head_camera->setMainPlayer(true);
    m_tail_camera->setMainPlayer(false);

    // for (auto it : m_trains) it->changeMoveDirection();
  }

  if (is_local_camera_switch && m_trains[0]->getMoveDirection() ==
                                    TrainController::MOVE_DIRECTION::REVERSE) {
    m_world_camera->setMainPlayer(false);
    m_head_camera->setMainPlayer(false);
    m_tail_camera->setMainPlayer(true);
  }

  SplineSegmentPtr aux_segment =
      (*m_trains.front())->getFBogiePos().m_curr_segment;
  Math::Vec3 forward = Math::Vec3_one;
  if (aux_segment)
    forward = aux_segment->calcPoint(
        (*m_trains.front())->getFBogiePos().m_t_coordinate);

  aux_segment = (*m_trains.front())->getBBogiePos().m_curr_segment;
  Math::Vec3 back = Math::Vec3_one;
  if (aux_segment)
    back = aux_segment->calcPoint(
        (*m_trains.front())->getBBogiePos().m_t_coordinate);

  m_head_camera->setWorldPosition((forward - back) / 2 + back);

  aux_segment = (*m_trains.back())->getFBogiePos().m_curr_segment;

  if (aux_segment)
    forward = aux_segment->calcPoint(
        (*m_trains.back())->getFBogiePos().m_t_coordinate);
  else
    forward = Math::Vec3_one;

  aux_segment = (*m_trains.back())->getBBogiePos().m_curr_segment;
  if (aux_segment)
    back = aux_segment->calcPoint(
        (*m_trains.back())->getBBogiePos().m_t_coordinate);
  else
    back = Math::Vec3_one;

  m_tail_camera->setWorldPosition((forward - back) / 2 + back);
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
