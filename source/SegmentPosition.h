#pragma once

#include <UnigineWorlds.h>

/**
 * @struct SegmentPosition
 * @brief Иммутабельный класс для хранения и расчёта информации о положении
 * тележек поезда.
 *
 * Эта структура используется для описания текущей позиции тележки на пути,
 * включая текущий сегмент сплайна, параметрическое положение на сегменте и
 * длину сегмента.
 */
struct SegmentPosition {
 public:
  /**
   * @brief Конструктор по умолчанию.
   *
   * Инициализирует структуру с пустыми значениями.
   */
  SegmentPosition()
      : m_curr_segment(nullptr), m_t_coordinate(0.f), m_curr_segment_len(0.f) {}

  /**
   * @brief Конструктор с параметрами.
   *
   * Инициализирует структуру с заданным сегментом сплайна и параметрической
   * координатой.
   * @param segment Указатель на сегмент сплайна.
   * @param t_coordinate Параметрическая координата на сегменте.
   */
  SegmentPosition(Unigine::SplineSegmentPtr segment, float t_coordinate)
      : m_curr_segment(segment),
        m_t_coordinate(t_coordinate),
        m_curr_segment_len(segment->getLength()) {}

  /**
   * @brief Получение указателя на текущий сегмент сплайна.
   * @return Указатель на текущий сегмент сплайна.
   */
  Unigine::SplineSegmentPtr getSegment() const { return m_curr_segment; }

  /**
   * @brief Получение позиции в мировом пространстве.
   * @return Позиция в мировом пространстве.
   */
  Unigine::Math::Vec3 getWorldPosition() const;

  /**
   * @brief Получение направления движения.
   * @return Направление движения.
   */
  Unigine::Math::vec3 getDirection() const;

  /**
   * @brief Перемещение на заданное расстояние.
   * @param distance Расстояние для перемещения.
   * @return Новая позиция после перемещения.
   */
  SegmentPosition moveBy(float distance) const;

  /**
   * @brief Вычисление позиции по длине сегмента.
   * @param length Длина сегмента.
   * @return Новая позиция, рассчитанная по длине сегмента.
   */
  SegmentPosition calcBySegmentLength(float length) const;

  /**
   * @brief Вычисление позиции по расстоянию.
   * @param distance Расстояние.
   * @return Новая позиция, рассчитанная по расстоянию.
   */
  SegmentPosition calcByDistance(float distance) const;

  /**
   * @brief Проверка на пустоту структуры.
   * @return true, если структура пустая, иначе false.
   */
  bool isEmpty() const;

  /**
   * @brief Установка коллбэк-функции для получения предыдущего сегмента.
   * @param fp Функция для получения предыдущего сегмента.
   */
  static void setFuncGetPrevSegment(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp);

  /**
   * @brief Установка коллбэк-функции для получения следующего сегмента.
   * @param fp Функция для получения следующего сегмента.
   */
  static void setFuncGetNextSegment(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp);

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
