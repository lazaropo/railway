#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>

#include "SegmentPosition.h"

/**
 * @class Bogie
 * @brief Класс для управления тележкой поезда.
 *
 * Этот класс наследует Unigine::ComponentBase и используется для управления
 * тележкой поезда. Он включает в себя методы для получения и установки позиции
 * тележки на пути, а также для обновления состояния тележки.
 */
class Bogie : public Unigine::ComponentBase {
 public:
  /**
   * @brief Определение компонента.
   *
   * Этот макрос определяет компонент Bogie, наследующий от
   * Unigine::ComponentBase.
   */
  COMPONENT_DEFINE(Bogie, Unigine::ComponentBase);

  /**
   * @brief Инициализация компонента.
   *
   * Этот макрос определяет функцию init() как функцию инициализации
   * компонента.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Обновление состояния компонента.
   *
   * Этот макрос определяет функцию update() как функцию обновления
   * состояния компонента.
   */
  COMPONENT_UPDATE(update);

  /**
   * @brief Получение позиции тележки в мировых координатах.
   *
   * Эта функция возвращает позицию тележки в мировых координатах.
   *
   * @return Позиция тележки в мировых координатах.
   */
  Unigine::Math::Vec3 getWorldPosition() const;

  /**
   * @brief Получение позиции тележки на пути.
   *
   * Эта функция возвращает позицию тележки на пути, используя структуру
   * SegmentPosition.
   *
   * @return Позиция тележки на пути.
   */
  SegmentPosition getSegmentPosition() const;

  /**
   * @brief Установка позиции тележки на пути.
   *
   * Эта функция устанавливает позицию тележки на пути, используя структуру
   * SegmentPosition.
   *
   * @param pos Новая позиция тележки на пути.
   */
  void setSegmentPosition(const SegmentPosition& pos);

  void setRotation(float shift);

 protected:
  /**
   * @brief Инициализация компонента.
   *
   * Эта функция вызывается при инициализации компонента.
   */
  void init();

  /**
   * @brief Обновление состояния компонента.
   *
   * Эта функция вызывается при каждом обновлении состояния компонента.
   */
  void update();

 public:
  /**
   * @brief Текущая позиция тележки на пути.
   *
   * Эта переменная хранит текущую позицию тележки на пути, используя структуру
   * SegmentPosition.
   */
  SegmentPosition m_position;

 protected:
  float m_circumference;
  float m_path;
  const float TORQUE_COEFFICIENT = 0.2;
};
