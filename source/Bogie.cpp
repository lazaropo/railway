#include "Bogie.h"

/**
 * @brief Регистрация компонента Bogie.
 *
 * Этот макрос регистрирует компонент Bogie в системе компонентов Unigine.
 */
REGISTER_COMPONENT(Bogie);

using namespace Unigine;

void Bogie::init() {
  int wheel_right = getNode()->findChild("WheelR");
  m_circumference =
      getNode()->getChild(wheel_right)->getBoundBox().getSize().y *
      Math::Consts::PI2;
}

/**
 * @brief Обновление состояния компонента Bogie.
 *
 * Эта функция вызывается при каждом обновлении состояния компонента Bogie.
 * Она обновляет позицию и направление узла в соответствии с текущей позицией
 * тележки на пути.
 */
void Bogie::update() {
  if (m_position.isEmpty()) return;

  // Получаем текущую позицию тележки в мировых координатах
  Math::Vec3 coordinate = m_position.getWorldPosition();

  // Получаем текущее направление тележки
  Math::vec3 direction = m_position.getDirection();

  float angle = 360.f * m_path / m_circumference;

  // Устанавливаем позицию и направление узла
  getNode()->setWorldPosition(coordinate);
  getNode()->setDirection(direction, Math::vec3_up, Math::AXIS_Y);
  getNode()->rotate(Math::quat(Math::vec3_right * direction, angle));

  // Визуализируем оси координат тележки
  Visualizer::renderVector(
      coordinate,
      coordinate + Math::Vec3(getNode()->getDirection(Math::AXIS_X)),
      Math::vec4_red);
  Visualizer::renderVector(
      coordinate,
      coordinate + Math::Vec3(getNode()->getDirection(Math::AXIS_Y)),
      Math::vec4_green);
  Visualizer::renderVector(
      coordinate,
      coordinate + Math::Vec3(getNode()->getDirection(Math::AXIS_Z)),
      Math::vec4_black);
}

/**
 * @brief Получение позиции тележки в мировых координатах.
 *
 * Эта функция возвращает позицию тележки в мировых координатах.
 *
 * @return Позиция тележки в мировых координатах.
 */
Math::Vec3 Bogie::getWorldPosition() const {
  return m_position.getWorldPosition();
}

/**
 * @brief Получение позиции тележки на пути.
 *
 * Эта функция возвращает позицию тележки на пути, используя структуру
 * SegmentPosition.
 *
 * @return Позиция тележки на пути.
 */
SegmentPosition Bogie::getSegmentPosition() const { return m_position; }

/**
 * @brief Установка позиции тележки на пути.
 *
 * Эта функция устанавливает позицию тележки на пути, используя структуру
 * SegmentPosition.
 *
 * @param pos Новая позиция тележки на пути.
 */
void Bogie::setSegmentPosition(const SegmentPosition& pos) { m_position = pos; }

void Bogie::setRotation(float shift) {
  m_path += shift * TORQUE_COEFFICIENT;
  while (m_path > m_circumference) m_path -= m_circumference;
}