#pragma once

#include <UnigineComponentSystem.h>

#include <memory>

#include "Carriage.h"
#include "MovementLogic.h"
#include "TrainManager.h"

/**
 * @class CarriagePlayerDummy
 * @brief Класс, наследующийся от Carriage, для управления составом с постоянной
 * скоростью.
 *
 * Этот класс предоставляет реализацию управления поездом, где скорость
 * движения задается константой.
 */
class CarriagePlayerDummy : public Carriage {
 public:
  /**
   * @brief Определяем тип компонента как CarriagePlayerDummy.
   */
  COMPONENT_DEFINE(CarriagePlayerDummy, Carriage);

  /**
   * @brief Параметр для постоянной скорости движения поезда.
   */
  PROP_PARAM(Float, const_move_speed, 20.f);

  /**
   * @brief Инициализация компонента.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Обновление компонента на каждом кадре.
   */
  COMPONENT_UPDATE(update);

 protected:
  /**
   * @brief Устанавливает начальную позицию для состава.
   *
   * Переопределяемая виртуальная функция для настройки стартовой позиции
   * состава.
   */
  void setStartPos() override;

  /**
   * @brief Внутренний метод инициализации.
   */
  void init();

  /**
   * @brief Внутренний метод обновления, вызываемый на каждом кадре.
   */
  void update();
};