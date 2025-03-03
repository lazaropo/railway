#include "Carriage.h"

/**
 * @brief Регистрирует компонент Carriage в системе компонентов Unigine.
 */
REGISTER_COMPONENT(Carriage);

using namespace Unigine;

/**
 * @brief Инициализация компонента Carriage.
 *
 * Эта функция вызывается при инициализации компонента. Она находит узел тела
 * вагона, получает компоненты Bogie (тележки) и вычисляет расстояние между
 * ними.
 */
void Carriage::init() {
  // Находим узел тела вагона
  m_body = getNode()->findNode("Body", true);

  // Получаем компоненты Bogie (тележки) из дочерних узлов
  Vector<Bogie*> bogies;
  ComponentSystem::get()->getComponentsInChildren<Bogie>(getNode(), bogies);

  // Проверяем, что у вагона ровно две тележки
  if (bogies.size() != 2)
    Log::error("Carriage component has %d Bogie children!\n", bogies.size());

  // Устанавливаем указатели на переднюю и заднюю тележки
  m_forward_bogie = std::make_unique<Bogie*>(bogies.first());
  m_back_bogie = std::make_unique<Bogie*>(bogies.last());

  // Вычисляем расстояние между тележками
  m_distance_btw_bogie = ((*m_forward_bogie)->getNode()->getWorldPosition() -
                          (*m_back_bogie)->getNode()->getWorldPosition())
                             .length();
}

/**
 * @brief Обновление состояния компонента Carriage.
 *
 * Эта функция вызывается при каждом обновлении состояния компонента. Она
 * обновляет позицию и направление тела вагона на основе позиций передней и
 * задней тележек.
 */
void Carriage::update() {
  if (!m_forward_bogie || !m_back_bogie || !m_body) return;

  // Получаем позиции передней и задней тележек
  Math::Vec3 forward_position = (*m_forward_bogie)->getWorldPosition();
  Math::Vec3 back_position = (*m_back_bogie)->getWorldPosition();

  // Вычисляем позицию тела вагона как среднее между тележками
  m_body_position = (forward_position - back_position) * 0.5f + back_position;
  // Вычисляем направление тела вагона
  Math::vec3 body_direction = Math::vec3(forward_position - back_position);

  // Устанавливаем позицию и направление тела вагона
  m_body->setWorldPosition(m_body_position);
  m_body->setDirection(body_direction, Math::vec3_up, Math::AXIS_Y);
}

/**
 * @brief Получение позиции тележки на пути.
 *
 * Эта функция возвращает позицию тележки на пути в зависимости от направления
 * движения. При движении вперед (FORWARD) возвращается позиция передней
 * тележки, при движении назад (REVERSE) — задней тележки.
 *
 * @param dir Направление движения (FORWARD или REVERSE).
 * @return Позиция тележки на пути.
 */
SegmentPosition Carriage::getSegmentPosition(const MOVE_DIRECTION& dir) const {
  if (dir == MOVE_DIRECTION::FORWARD)
    return (*m_forward_bogie)->getSegmentPosition();
  else
    return (*m_back_bogie)->getSegmentPosition();
}

/**
 * @brief Получение позиции вагона в мировых координатах.
 *
 * Эта функция возвращает позицию тела вагона в мировых координатах.
 *
 * @return Позиция вагона в мировых координатах.
 */
Unigine::Math::Vec3 Carriage::getWorldPosition() const {
  return m_body_position;
}

/**
 * @brief Установка позиции вагона.
 *
 * Эта функция устанавливает позицию вагона на пути в зависимости от направления
 * движения. При движении вперед (FORWARD) сначала устанавливается позиция
 * передней тележки, а затем задней. При движении назад (REVERSE) — наоборот.
 *
 * @param pos Новая позиция вагона на пути.
 * @param dir Направление движения (FORWARD или REVERSE).
 */
void Carriage::setPosition(SegmentPosition pos, const MOVE_DIRECTION& dir) {
  if (!m_forward_bogie || !m_back_bogie) return;

  // Устанавливаем позиции тележек в зависимости от направления движения
  if (dir == MOVE_DIRECTION::FORWARD) {
    (*m_forward_bogie)->setSegmentPosition(pos);
    (*m_back_bogie)
        ->setSegmentPosition(pos.calcByDistance(-m_distance_btw_bogie));
  } else {
    (*m_back_bogie)->setSegmentPosition(pos);
    (*m_forward_bogie)
        ->setSegmentPosition(pos.calcByDistance(m_distance_btw_bogie));
  }
}

/**
 * @brief Получение длины вагона.
 *
 * Эта функция возвращает длину вагона, рассчитанную на основе размера его тела.
 *
 * @return Длина вагона.
 */
float Carriage::getLength() const {
  if (m_body)
    return m_body->getHierarchyBoundBox().getSize().y;
  else
    return 0.f;
}