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
struct SegmentPosition {
 public:
  SegmentPosition()
      : m_curr_segment(nullptr), m_t_coordinate(0.f), m_curr_segment_len(0.f) {}

  SegmentPosition(Unigine::SplineSegmentPtr segment, float t_coordinate)
      : m_curr_segment(segment),
        m_t_coordinate(t_coordinate),
        m_curr_segment_len(segment->getLength()) {}

  Unigine::SplineSegmentPtr getSegment() const { return m_curr_segment; }

  Unigine::Math::Vec3 getWorldPosition() const;

  Unigine::Math::vec3 getDirection() const;

  SegmentPosition moveBy(float distance) const;

  SegmentPosition calcBySegmentLength(float length) const;

  SegmentPosition calcByDistance(float distance) const;

  bool isEmpty() const { return !m_curr_segment || !m_curr_segment_len; }

  inline static void setFuncGetPrevSegment(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
    m_callback_prev_segment_f = fp;
  }

  inline static void setFuncGetNextSegment(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
    m_callback_next_segment_f = fp;
  }

 private:
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
};