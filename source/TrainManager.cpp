#include "TrainManager.h"

/**
 * @brief Регистрирует компонент TrainManager в системе компонентов Unigine.
 */
REGISTER_COMPONENT(TrainManager);

using namespace Unigine;

/**
 * @brief Статический указатель на единственный экземпляр менеджера поездов.
 */
TrainManager* TrainManager::m_instance = nullptr;

/**
 * @brief Инициализация компонента TrainManager.
 *
 * Этот метод создает единственный экземпляр менеджера поездов и проверяет
 * наличие контейнера путей. Если контейнер путей не найден, выбрасывается
 * исключение.
 */
void TrainManager::init() {
  if (!m_instance)
    m_instance = this;
  else
    return;

  // Получаем компонент TrackContainer из системы компонентов
  m_track_container = std::unique_ptr<TrackContrainer>(
      ComponentSystem::get()->getComponent<TrackContrainer>(track_container));
  if (!m_track_container)
    throw std::runtime_error(
        "There are not any node with assigned TrackContainer property.\n");

  // Задаём позицию первому поезду.
  Train* first_train = ComponentSystem::get()->getComponentInChildren<Train>(
      train_container[0].get());
  if (first_train) first_train->setStartPosition(getStartSegment());

  // Передаём классу функции для получения предыдущего/следующего сегмента.
  SegmentPosition::setFuncGetNextSegment(&TrainManager::getNextSegment);
  SegmentPosition::setFuncGetPrevSegment(&TrainManager::getPrevSegment);
}

/**
 * @brief Возвращает начальный сегмент пути.
 *
 * Этот метод возвращает указатель на начальный сегмент пути.
 *
 * @return Указатель на начальный сегмент пути.
 */
SplineSegmentPtr TrainManager::getStartSegment() {
  return TrainManager::getInstance()
      ->m_track_container->getSplineSegments()[80];
}

/**
 * @brief Возвращает следующий сегмент пути относительно текущего.
 *
 * Этот метод ищет следующий сегмент пути, исходя из конечной точки текущего
 * сегмента. Если следующий сегмент не найден, возвращается nullptr.
 *
 * @param curr_segment Указатель на текущий сегмент пути.
 * @return Указатель на следующий сегмент пути.
 */
Unigine::SplineSegmentPtr TrainManager::getNextSegment(
    Unigine::SplineSegmentPtr curr_segment) {
  if (!curr_segment) return nullptr;

  SplineSegmentPtr next_segment = nullptr;
  Math::Vec3 curr_end_point = curr_segment->getEndPoint()->getPosition();

  // Проходим по всем сегментам пути
  for (auto it :
       TrainManager::getInstance()->m_track_container->getSplineSegments()) {
    if (curr_segment == it) continue;

    // Проверяем, совпадает ли начальная точка текущего сегмента с конечной
    // точкой другого сегмента
    if ((curr_end_point - it->getStartPoint()->getPosition()).length() <
        Math::Consts::EPS) {
      next_segment = it;
      break;
    }
  }

  return next_segment;
}

/**
 * @brief Возвращает предыдущий сегмент пути относительно текущего.
 *
 * Этот метод ищет предыдущий сегмент пути, исходя из начальной точки текущего
 * сегмента. Если предыдущий сегмент не найден, возвращается nullptr.
 *
 * @param curr_segment Указатель на текущий сегмент пути.
 * @return Указатель на предыдущий сегмент пути.
 */
Unigine::SplineSegmentPtr TrainManager::getPrevSegment(
    Unigine::SplineSegmentPtr curr_segment) {
  if (!curr_segment) return nullptr;

  SplineSegmentPtr prev_segment = nullptr;
  Math::Vec3 curr_start_point = curr_segment->getStartPoint()->getPosition();

  // Проходим по всем сегментам пути
  for (auto it :
       TrainManager::getInstance()->m_track_container->getSplineSegments()) {
    if (curr_segment == it) {
      // is_current_exists = true;
      continue;
    }
    // Проверяем, совпадает ли конечная точка текущего сегмента с начальной
    // точкой другого сегмента
    if ((curr_start_point - it->getEndPoint()->getPosition()).length() <
        Math::Consts::EPS) {
      prev_segment = it;
      break;
    }
  }

  return prev_segment;
}
