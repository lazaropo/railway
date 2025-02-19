#pragma once

#include <UnigineWorlds.h>

/**
 * @struct BogiePos
 * @brief Структура для хранения информации о положении тележек поезда.
 *
 * Эта структура используется для описания текущей позиции тележки на пути,
 * включая текущий сегмент сплайна, параметрическое положение на сегменте и
 * длину сегмента.
 */
struct BogiePos {
  /**
   * @brief Указатель на текущий сегмент сплайна.
   */
  Unigine::SplineSegmentPtr m_curr_segment;

  /**
   * @brief Текущая параметрическая координата t на сегменте.
   */
  float m_t_coordinate = 0;

  /**
   * @brief Длина текущего сегмента.
   */
  float m_curr_segment_len = 0;
};