#include "Carriage.h"

using namespace Unigine;

/**
 * @brief Регистрирует компонент Carriage в системе компонентов Unigine.
 */
REGISTER_COMPONENT(Carriage);

/**
 * @brief Устанавливает поезда, связанные с данным составом.
 *
 * Этот метод ищет все поезда среди дочерних узлов текущего узла
 * и устанавливает для каждого из них функции обратного вызова для старта и
 * остановки движения всего состава.
 */
void Carriage::setTrains() {
  NodePtr node = getNode();

  // Поиск всех поездов среди дочерних узлов
  getComponentsInChildren<TrainController>(getNode(), m_trains);

  // Настройка функций обратного вызова для каждого найденного поезда
  for (auto train : m_trains) {
    train->setMoveStartFunc(
        std::bind(&Carriage::startMove, this));  // Функция для запуска движения
    train->setMoveEndFunc(std::bind(&Carriage::stopMove,
                                    this));  // Функция для остановки движения
  }
}