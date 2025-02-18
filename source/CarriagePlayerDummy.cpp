#include "CarriagePlayerDummy.h"

REGISTER_COMPONENT(CarriagePlayerDummy);

using namespace Unigine;

void CarriagePlayerDummy::init() {
  Vector<NodePtr> root_nodes;
  World::getRootNodes(root_nodes);

  setTrains();

  TrainManager::setCarriage(this);
  // TrainManager* p_train_manager = nullptr;
  // for (auto& it : root_nodes) {
  //   if (p_train_manager =
  //           ComponentSystem::get()->getComponentInChildren<TrainManager>(it))
  //           {
  //     //p_train_manager->setCarriage(this);

  //     break;
  //   }
  // }

  if (const_move_speed > Math::Consts::EPS)
    for (TrainController* train : m_trains)
      train->start_speed = const_move_speed;

  if (m_trains.size()) {
    std::shared_ptr<IMovementLogic> logic(new MovementLogic());
    logic->setPrevSegmentFunction(&TrainManager::getPrevSegment);
    logic->setNextSegmentFunction(&TrainManager::getNextSegment);
    logic->setGetIfpsFunction(std::bind(&Carriage::getIfps, this));

    for (TrainController* train : m_trains) train->setMovementLogic(logic);
    // train->start_speed = const_move_speed;
    // m_trains[0]->setPrevSegmentFunction(//std::bind(
    //     &TrainManager::getPrevSegment/*, TrainManager::getInstance(),
    //     std::placeholders::_1)*/);
    // m_trains[0]->setNextSegmentFunction(&TrainManager::getNextSegment);
  }
}
