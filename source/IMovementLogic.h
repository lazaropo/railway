#pragma once

#include <UnigineWorlds.h>

#include <functional>

#include "BogiePos.h"

/**
 * @class IMovementLogic
 * @brief Интерфейс для логики перемещения поезда.
 *
 * Этот интерфейс описывает методы для перемещения первой тележки и расчета
 * положения второй тележки относительно первой. Также позволяет устанавливать
 * коллбэк-функции для получения предыдущего и следующего сегментов пути, а
 * также значения IFPS.
 *
 * Первая тележка - передняя при прямом движении, задняя при обратной
 * направлении движения.
 */
class IMovementLogic {
 public:
  /**
   * @brief Перемещает первую тележку.
   * @param first Информация о первой тележке.
   * @param v_speed Скорость движения.
   * @return Новая информация о положении первой тележки.
   */
  virtual BogiePos moveFirstBogie(BogiePos first, float v_speed) = 0;

  /**
   * @brief Рассчитывает положение второй тележки относительно первой.
   * @param first Информация о первой тележке.
   * @param second Информация о второй тележке.
   * @param distance_btw_bogie Расстояние между тележками.
   * @return Новая информация о положении второй тележки.
   */
  virtual BogiePos calcSecondBogiePos(const BogiePos& first, BogiePos second,
                                      float distance_btw_bogie) = 0;

  /**
   * @brief Устанавливает коллбэк-функцию для получения предыдущего сегмента
   * пути.
   * @param fp Указатель на функцию обратного вызова.
   */
  void setPrevSegmentFunction(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
    m_callback_prev_segment_f = fp;
  }

  /**
   * @brief Устанавливает коллбэк-функцию для получения следующего сегмента
   * пути.
   * @param fp Указатель на функцию обратного вызова.
   */
  void setNextSegmentFunction(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
    m_callback_next_segment_f = fp;
  }

  /**
   * @brief Устанавливает коллбэк-функцию для получения значения IFPS.
   * @param fp Указатель на функцию обратного вызова.
   */
  void setGetIfpsFunction(std::function<float()> fp) {
    m_callback_get_ifps_f = fp;
  }

 protected:
  /**
   * @brief Коллбэк-функция для получения предыдущего сегмента пути.
   */
  inline static std::function<Unigine::SplineSegmentPtr(
      Unigine::SplineSegmentPtr)>
      m_callback_prev_segment_f = nullptr;

  /**
   * @brief Коллбэк-функция для получения следующего сегмента пути.
   */
  inline static std::function<Unigine::SplineSegmentPtr(
      Unigine::SplineSegmentPtr)>
      m_callback_next_segment_f = nullptr;

  /**
   * @brief Коллбэк-функция для получения значения IFPS.
   */
  inline static std::function<float()> m_callback_get_ifps_f = nullptr;
};