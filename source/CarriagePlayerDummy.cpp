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

  for (TrainController* it : m_trains) {
    it->start_speed = const_move_speed;
  }

  if (m_trains.size())
    m_trains[0]->setPrevSegmentFunction(//std::bind(
        &TrainManager::getPrevSegment/*, TrainManager::getInstance(), std::placeholders::_1)*/);
}
