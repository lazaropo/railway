#pragma once

#include "IMovementLogic.h"
#include "TrainController.h"

/**
 * @class MovementLogic
 * @brief Реализация интерфейса IMovementLogic для логики перемещения поезда.
 *
 * Этот класс реализует методы для перемещения первой тележки и расчета
 * положения второй тележки относительно первой.
 */
class MovementLogic : public IMovementLogic {
 public:
  /**
   * @brief Перемещает первую тележку.
   * @param first Информация о первой тележке.
   * @param v_speed Скорость движения.
   * @return Новая информация о положении первой тележки.
   */
  BogiePos moveFirstBogie(BogiePos first, float v_speed) override;

  /**
   * @brief Рассчитывает положение второй тележки относительно первой.
   * @param first Информация о первой тележке.
   * @param second Информация о второй тележке.
   * @param distance_btw_bogie Расстояние между тележками.
   * @return Новая информация о положении второй тележки.
   */
  BogiePos calcSecondBogiePos(const BogiePos& first, BogiePos second,
                              float distance_btw_bogie) override;
};