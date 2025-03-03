#include "Carriage.h"

/**
 * @brief Регистрирует компонент Carriage в системе компонентов Unigine.
 */
REGISTER_COMPONENT(Carriage);

using namespace Unigine;

void Carriage::init() {
  // NodeReferencePtr ref =
  // checked_ptr_cast<NodeReference>(carriage_node.get()); if (ref) {
  //   m_body = ref;
  m_body = getNode()->findNode("Body", true);

  Vector<Bogie*> bogies;
  ComponentSystem::get()->getComponentsInChildren<Bogie>(getNode(), bogies);

  if (bogies.size() != 2)
    Log::error("Carriage component has %d Bogie children!\n", bogies.size());

  m_forward_bogie = std::make_unique<Bogie*>(bogies.first());
  m_back_bogie = std::make_unique<Bogie*>(bogies.last());

  // m_forward_bogie = std::make_unique<Bogie>(bogies.first());
  // m_back_bogie = std::make_unique<Bogie>(bogies.last());

  m_distance_btw_bogie = ((*m_forward_bogie)->getNode()->getWorldPosition() -
                          (*m_back_bogie)->getNode()->getWorldPosition())
                             .length();
  //}
}

void Carriage::update() {
  if (/*m_position.isEmpty() || */ !m_forward_bogie || !m_back_bogie || !m_body)
    return;

  Math::Vec3 forward_position = (*m_forward_bogie)->getWorldPosition();
  Math::Vec3 back_position = (*m_back_bogie)->getWorldPosition();

  Math::Vec3 body_position =
      (forward_position - back_position) * 0.5f + back_position;
  Math::vec3 body_direction = Math::vec3(forward_position - back_position);

  m_body->setWorldPosition(body_position);
  m_body->setDirection(body_direction, Math::vec3_up, Math::AXIS_Y);
}

void Carriage::setPosition(SegmentPosition pos) {
  if (!m_forward_bogie || !m_back_bogie) return;
  (*m_forward_bogie)->setSegmentPosition(pos);
  (*m_back_bogie)
      ->setSegmentPosition(pos.calcByDistance(-m_distance_btw_bogie));
}

// void Carriage::setStartPosition(SegmentPosition pos) {
//   m_forward_bogie->setStartPosition(pos);
// }

// /**
//  * @brief Устанавливает поезда, связанные с данным составом.
//  *
//  * Этот метод ищет все поезда среди дочерних узлов текущего узла
//  * и устанавливает для каждого из них функции обратного вызова для старта и
//  * остановки движения всего состава.
//  */
// void Carriage::setTrains() {
//   NodePtr node = getNode();

//   // Поиск всех поездов среди дочерних узлов
//   getComponentsInChildren<TrainController>(getNode(), m_trains);

//   // Настройка функций обратного вызова для каждого найденного поезда
//   for (auto train : m_trains) {
//     train->setMoveStartFunc(
//         std::bind(&Carriage::startMove, this));  // Функция для запуска
//         движения
//     train->setMoveEndFunc(std::bind(&Carriage::stopMove,
//                                     this));  // Функция для остановки
//                                     движения
//   }
// }