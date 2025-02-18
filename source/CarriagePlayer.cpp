#include "CarriagePlayer.h"

REGISTER_COMPONENT(CarriagePlayer);

using namespace Unigine;

void CarriagePlayer::init() {
  Vector<NodePtr> root_nodes;
  World::getNodes(root_nodes);

  TrainManager::setCarriage(this);
  setTrains();

  if (m_trains.size()) {
    std::shared_ptr<IMovementLogic> logic(new MovementLogic());
    logic->setPrevSegmentFunction(&TrainManager::getPrevSegment);
    logic->setNextSegmentFunction(&TrainManager::getNextSegment);
    logic->setGetIfpsFunction(std::bind(&Carriage::getIfps, this));

    for (TrainController* train : m_trains) train->setMovementLogic(logic);

    Log::message("Carriage Player: %d trains are load\n", m_trains.size());
  }

  m_head_camera = checked_ptr_cast<PlayerSpectator>(head_camera.get());
  m_tail_camera = checked_ptr_cast<PlayerSpectator>(tail_camera.get());
  m_world_camera = checked_ptr_cast<PlayerSpectator>(world_camera.get());

  if (m_head_camera) m_head_camera->setMainPlayer(true);
}

void CarriagePlayer::update() {
  if (!m_trains[0]->getCurrentSegment()) setStartPos();

  float tmp = 0;

  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_DECREASE);
  if (Math::abs(tmp) > Math::Consts::EPS)
    for (auto it : m_trains) it->brake();

  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_INCREASE);
  if (Math::abs(tmp) > Math::Consts::EPS)
    for (auto it : m_trains) it->accelerate();

  bool is_local_camera_switch = false;

  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::TURN_AROUND)) {
    for (auto train : m_trains) {
      train->changeMoveDirection();
      train->startMove();
    }
    is_local_camera_switch = true;
  }

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
  }

  if (is_local_camera_switch && m_trains[0]->getMoveDirection() ==
                                    TrainController::MOVE_DIRECTION::REVERSE) {
    m_world_camera->setMainPlayer(false);
    m_head_camera->setMainPlayer(false);
    m_tail_camera->setMainPlayer(true);
  }

  m_head_camera->setWorldPosition(
      (*m_trains.front())->getNode()->getPosition() + Math::Vec3_up * 5);
  m_tail_camera->setWorldPosition((*m_trains.back())->getNode()->getPosition() +
                                  Math::Vec3_up * 5);
}

void CarriagePlayer::setStartPos() {
  if (m_trains.empty()) return;

  SplineSegmentPtr current_segment = TrainManager::getStartSegment();
  SplineSegmentPtr prev_segment = TrainManager::getPrevSegment(current_segment);

  int len = current_segment->getLength();
  float pos = 0.f;

  for (auto it = m_trains.begin(), it_end = m_trains.end(); it != it_end;
       ++it) {
    if (!current_segment) break;

    (*it)->setTrainAtSegment(current_segment, prev_segment, pos);

    if (Math::abs(len) < Math::Consts::EPS) continue;

    pos -= ((*it)->getLength() + 2.f) / len;
    if (pos < 0.f) {
      current_segment = prev_segment;
      prev_segment = TrainManager::getPrevSegment(current_segment);

      pos = 1.f - (-pos) * len / current_segment->getLength();
      len = current_segment->getLength();
    }
  }
}
