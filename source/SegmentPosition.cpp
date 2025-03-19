#include "SegmentPosition.h"

using namespace Unigine;

/**
 * @brief Перемещение по пути (сегмент сплайна) на заданное расстояние (не
 * параметрическое).
 *
 * Функция перемещает объект по пути на заданное расстояние. Если расстояние
 * положительное, объект перемещается вперед, если отрицательное — назад.
 * Если объект достигает конца пути, перемещение прекращается.
 *
 * @param distance Расстояние для перемещения.
 * @return Новая позиция после перемещения.
 */
SegmentPosition SegmentPosition::moveBy(float distance) const {
  SegmentPosition ret_pos = *this;

  SplineSegmentPtr segment = m_curr_segment;
  distance /= m_curr_segment_len;
  ret_pos.m_t_coordinate += distance;

  // Если тележка движется вперед
  if (distance > 0.f) {
    while (ret_pos.m_t_coordinate - 1.f > Math::Consts::EPS &&
           ret_pos.m_curr_segment) {
      // Переход на следующий сегмент пути
      segment = m_callback_next_segment_f(segment);

      if (segment) {
        // Перерасчет параметра t для нового сегмента
        ret_pos.m_t_coordinate = (ret_pos.m_t_coordinate - 1.f) *
                                 ret_pos.m_curr_segment_len /
                                 segment->getLength();
        ret_pos.m_curr_segment = segment;
        ret_pos.m_curr_segment_len = segment->getLength();
      } else {
        // Достигнут конец пути или стрелка
        ret_pos.m_curr_segment = nullptr;
        ret_pos.m_t_coordinate = -1.f;
        ret_pos.m_curr_segment_len = 0;
      }
    }
  } else {
    // Тележка движется назад
    while (ret_pos.m_t_coordinate < 0.f && ret_pos.m_curr_segment) {
      // Переход на предыдущий сегмент пути
      segment = m_callback_prev_segment_f(segment);

      if (segment) {
        // Перерасчет параметра t для нового сегмента
        ret_pos.m_t_coordinate = 1 + ret_pos.m_t_coordinate *
                                         ret_pos.m_curr_segment_len /
                                         segment->getLength();
        ret_pos.m_curr_segment = segment;
        ret_pos.m_curr_segment_len = segment->getLength();
      } else {
        // Попали в стрелку на путях поезда или в конец пути
        ret_pos.m_curr_segment = nullptr;
        ret_pos.m_t_coordinate = -1.f;
        ret_pos.m_curr_segment_len = 0;
      }
    }
  }

  return ret_pos;
}

/**
 * @brief Вычисление позиции по расстоянию в мировых координатах.
 *
 * Функция вычисляет новую позицию на пути, исходя из заданного расстояния.
 * Если расстояние положительное, объект перемещается вперед, если отрицательное
 * — назад. Если объект достигает конца пути, перемещение прекращается.
 *
 * @param distance Расстояние для перемещения.
 * @return Новая позиция после перемещения.
 */
SegmentPosition SegmentPosition::calcByDistance(float distance) const {
  if (!m_curr_segment || m_curr_segment_len < Math::Consts::EPS)
    return SegmentPosition();
  if (Math::abs(distance) < Math::Consts::EPS)
    return SegmentPosition(m_curr_segment, m_t_coordinate);

  SegmentPosition ret_pos(m_curr_segment, m_t_coordinate);

  // Рассчитываем текущие позиции по точкам
  Math::Vec3 v_first = getWorldPosition();
  Math::Vec3 v_second = v_first;

  float calc_distance = 0.f;  // Текущее расстояние между точками
  float excess = calc_distance - Math::abs(distance);  // Разница в расстоянии
  int count = 0;  // Счетчик итераций

  // Итерируем до тех пор, пока разница в расстоянии не станет незначительной
  // или если число итераций дошло до 10. Это просто эмпирический параметр. Как
  // правило точность расчёта после 10й итерации не увеличивается.
  while (Math::abs(excess) > Math::Consts::EPS && count < 10) {
    if (distance > 0.f)
      ret_pos.m_t_coordinate -= excess / ret_pos.m_curr_segment_len;
    else
      ret_pos.m_t_coordinate += excess / ret_pos.m_curr_segment_len;

    // Проверяем выход за пределы текущего сегмента пути
    while ((ret_pos.m_t_coordinate - 1.f) > Math::Consts::EPS &&
           ret_pos.m_curr_segment) {
      // Переход на следующий сегмент пути
      ret_pos.m_curr_segment =
          m_callback_next_segment_f(ret_pos.m_curr_segment);

      if (!ret_pos.m_curr_segment) {
        // Возврат к начальному сегменту. Конец пути или стрелка.
        ret_pos.m_curr_segment = m_curr_segment;
        ret_pos.m_curr_segment_len = m_curr_segment_len;
        ret_pos.m_t_coordinate = 1.f;
        return ret_pos;
      } else {
        // Перерасчет параметра t для нового сегмента
        ret_pos.m_t_coordinate = (ret_pos.m_t_coordinate - 1.f) *
                                 ret_pos.m_curr_segment_len /
                                 ret_pos.m_curr_segment->getLength();
      }
      // Обновляем длину текущего сегмента
      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    }

    // Проверяем выход за начало текущего сегмента пути
    while (ret_pos.m_t_coordinate < 0.f && ret_pos.m_curr_segment) {
      // Переход на предыдущий сегмент пути
      ret_pos.m_curr_segment =
          m_callback_prev_segment_f(ret_pos.m_curr_segment);

      if (!ret_pos.m_curr_segment) {
        // Возврат к начальному сегменту. Конец пути или стрелка.
        ret_pos.m_curr_segment = m_curr_segment;
        ret_pos.m_curr_segment_len = m_curr_segment_len;
        ret_pos.m_t_coordinate = 0.f;
        return ret_pos;
      } else {
        // Перерасчет параметра t для нового сегмента
        ret_pos.m_t_coordinate = 1.f + ret_pos.m_t_coordinate *
                                           ret_pos.m_curr_segment_len /
                                           ret_pos.m_curr_segment->getLength();
      }
      // Обновляем длину текущего сегмента
      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    }
    // Рассчитываем новые позиции по точкам
    v_second = ret_pos.m_curr_segment->calcPoint(
        ret_pos.m_curr_segment->linearToParametric(ret_pos.m_t_coordinate));

    // Рассчитываем новое расстояние между точками
    calc_distance = (v_first - v_second).length();

    // Рассчитываем разницу в расстоянии
    excess = calc_distance - Math::abs(distance);

    // Увеличение итерации
    ++count;
  }

  return ret_pos;
}

/**
 * @brief Получение позиции в мировых координатах.
 *
 * Функция возвращает позицию объекта в мировых координатах.
 *
 * @return Позиция в мировых координатах.
 */
Math::Vec3 SegmentPosition::getWorldPosition() const {
  if (m_curr_segment)
    return m_curr_segment->calcPoint(
        m_curr_segment->linearToParametric(m_t_coordinate));
  else
    return Math::Vec3_zero;
}

/**
 * @brief Получение направления движения.
 *
 * Функция возвращает направление движения объекта.
 *
 * @return Направление движения.
 */
Math::vec3 SegmentPosition::getDirection() const {
  if (m_curr_segment)
    return m_curr_segment->calcTangent(
        m_curr_segment->linearToParametric(m_t_coordinate));
  else
    return Math::vec3_zero;
}

/**
 * @brief Проверка на пустоту структуры.
 *
 * Функция проверяет, является ли структура пустой (не содержит сегментов или
 * длины сегмента).
 *
 * @return true, если структура пустая, иначе false.
 */
bool SegmentPosition::isEmpty() const {
  return !m_curr_segment || !m_curr_segment_len;
}

/**
 * @brief Установка коллбэк-функции для получения предыдущего сегмента.
 *
 * Функция устанавливает коллбэк-функцию, которая будет вызываться для получения
 * предыдущего сегмента пути.
 *
 * @param fp Функция для получения предыдущего сегмента.
 */
void SegmentPosition::setFuncGetPrevSegment(
    std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
  m_callback_prev_segment_f = fp;
}

/**
 * @brief Установка коллбэк-функции для получения следующего сегмента.
 *
 * Функция устанавливает коллбэк-функцию, которая будет вызываться для получения
 * следующего сегмента пути.
 *
 * @param fp Функция для получения следующего сегмента.
 */
void SegmentPosition::setFuncGetNextSegment(
    std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
  m_callback_next_segment_f = fp;
}
