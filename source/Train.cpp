#include "Train.h"

/**
 * @brief Регистрирует компонент Train в системе компонентов Unigine.
 */
REGISTER_COMPONENT(Train);

using namespace Unigine;

/**
 * @brief Инициализация компонента Train.
 *
 * Эта функция вызывается при инициализации компонента. Она получает все
 * компоненты Carriage (вагоны) из дочерних узлов и вычисляет длину поезда.
 */
void Train::init() {
  // Получаем все компоненты Carriage (вагоны) из дочерних узлов
  Vector<Carriage*> carriages;
  ComponentSystem::get()->getComponentsInChildren<Carriage>(getNode(),
                                                            carriages);

  // Добавляем все вагоны в контейнер m_carriage
  for (auto carriage : carriages)
    m_carriage.push_back(std::make_shared<Carriage*>(carriage));

  // Если есть хотя бы один вагон, вычисляем длину поезда
  if (m_carriage.size()) {
    m_carraige_len = (*m_carriage[0])->getNode()->getBoundBox().getSize().y;
  }
}

/**
 * @brief Обновление состояния компонента Train.
 *
 * Эта функция вызывается при каждом обновлении состояния компонента. Она
 * обновляет позицию и направление поезда, а также управляет движением вагонов.
 */
void Train::update() {
  // Проверяем, не остановлен ли поезд и не пустая ли текущая позиция
  if (m_is_stop || m_position.isEmpty()) return;

  // Обновляем длину поезда
  m_carraige_len = (*m_carriage[0])->getLength();

  // Вычисляем смещение и расстояние для движения поезда
  float shift = Math::min(m_speed, max_speed) * Game::getIFps();
  float distance = shift;

  // Определяем начальную позицию поезда в зависимости от направления движения
  auto tmp_carriage = m_move_direction == Carriage::MOVE_DIRECTION::FORWARD
                          ? m_carriage.begin()
                          : --m_carriage.end();
  SegmentPosition tmp_segm_pos;

  // Получаем текущую позицию первого или последнего вагона
  tmp_segm_pos = (**tmp_carriage)->getSegmentPosition(m_move_direction);
  if (tmp_segm_pos.isEmpty()) {
    // Если позиция пустая, устанавливаем начальную позицию
    (**tmp_carriage)->setPosition(m_position, m_move_direction);
    tmp_segm_pos = m_position;
  }

  // Обновляем позицию поезда в зависимости от направления движения
  if (m_move_direction == Carriage::MOVE_DIRECTION::FORWARD) {
    m_position = tmp_segm_pos.moveBy(distance);
    distance -= margin + m_carraige_len;
  } else {
    distance = -distance;
    m_position = tmp_segm_pos.moveBy(distance);
    distance += margin + m_carraige_len;
  }

  // Обновляем позицию поезда
  tmp_segm_pos = m_position;

  // Проверяем, не достигли ли мы конца пути
  if (tmp_segm_pos.isEmpty()) {
    m_is_stop = true;
    return;
  }

  // Устанавливаем позиции всех вагонов в зависимости от направления движения
  if (m_move_direction == Carriage::MOVE_DIRECTION::FORWARD) {
    for (auto it = m_carriage.begin(), it_end = m_carriage.end(); it != it_end;
         ++it) {
      (**it)->setPosition(tmp_segm_pos, m_move_direction);
      tmp_segm_pos = tmp_segm_pos.calcByDistance(distance);
    }
  } else {
    for (auto it = m_carriage.rbegin(), it_end = m_carriage.rend();
         it != it_end; ++it) {
      (**it)->setPosition(tmp_segm_pos, m_move_direction);
      tmp_segm_pos = tmp_segm_pos.calcByDistance(distance);
    }
  }
}

/**
 * @brief Получение направления движения поезда.
 *
 * Эта функция возвращает текущее направление движения поезда.
 *
 * @return Направление движения поезда.
 */
Carriage::MOVE_DIRECTION Train::getMoveDirection() const {
  return m_move_direction;
}

/**
 * @brief Изменение направления движения поезда.
 *
 * Эта функция изменяет направление движения поезда на противоположное.
 */
void Train::changeMoveDirection() {
  m_is_stop = false;
  if (m_move_direction == Carriage::MOVE_DIRECTION::FORWARD) {
    m_move_direction = Carriage::MOVE_DIRECTION::REVERSE;
    m_position = (**m_carriage.back())
                     ->getSegmentPosition(Carriage::MOVE_DIRECTION::REVERSE);
  } else {
    m_move_direction = Carriage::MOVE_DIRECTION::FORWARD;
    m_position = (**m_carriage.front())
                     ->getSegmentPosition(Carriage::MOVE_DIRECTION::FORWARD);
  }
}

/**
 * @brief Торможение поезда.
 *
 * Эта функция инициирует процесс торможения поезда, уменьшая его скорость.
 */
void Train::brake() {
  m_speed = m_speed - m_speed_delta > Unigine::Math::Consts::EPS
                ? m_speed - m_speed_delta
                : 0.f;
}

/**
 * @brief Ускорение поезда.
 *
 * Эта функция инициирует процесс ускорения поезда, увеличивая его скорость.
 */
void Train::accelerate() {
  m_speed =
      m_speed + m_speed_delta < max_speed ? m_speed + m_speed_delta : max_speed;
}

/**
 * @brief Получение позиции передней части поезда в мировых координатах.
 *
 * Эта функция возвращает позицию передней части поезда в мировых координатах.
 * Возвращает координаты тары первого вагона.
 *
 * @return Позиция передней части поезда в мировых координатах.
 */
Math::Vec3 Train::getFrontWorldPosition() const {
  return (**m_carriage.front())->getWorldPosition();
}

/**
 * @brief Получение позиции задней части поезда в мировых координатах.
 *
 * Эта функция возвращает позицию задней части поезда в мировых координатах.
 * Возвращает координаты тары последнего вагона.
 *
 * @return Позиция задней части поезда в мировых координатах.
 */
Math::Vec3 Train::getBackWorldPosition() const {
  return (**m_carriage.back())->getWorldPosition();
}

/**
 * @brief Установка начальной позиции поезда.
 *
 * Эта функция устанавливает начальную позицию поезда на заданном сегменте пути.
 * Устанавливает переднюю тележку первого вагона на переданный сегмент.
 *
 * @param segment Указатель на сегмент пути.
 * @param t_coordinate Параметрическая координата на сегменте (по умолчанию 0).
 */
void Train::setStartPosition(SplineSegmentPtr segment, float t_coordinate) {
  SegmentPosition segm_pos(segment, t_coordinate);

  m_position = segm_pos;
}
