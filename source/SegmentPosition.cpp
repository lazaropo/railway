#include "SegmentPosition.h"

using namespace Unigine;

SegmentPosition SegmentPosition::moveBy(float distance) const {
  SegmentPosition ret_bogie = *this;

  SplineSegmentPtr segment = m_curr_segment;
  distance /= m_curr_segment_len;
  ret_bogie.m_t_coordinate += distance;

  // Если первая тележка движется вперед
  if (distance > 0.f) {
    while (ret_bogie.m_t_coordinate - 1.f > Math::Consts::EPS &&
           ret_bogie.m_curr_segment) {
      // Переход на следующий сегмент пути
      segment = m_callback_next_segment_f(segment);

      if (segment) {
        // Перерасчет параметра t для нового сегмента
        ret_bogie.m_t_coordinate = (ret_bogie.m_t_coordinate - 1.f) *
                                   ret_bogie.m_curr_segment_len /
                                   segment->getLength();
        ret_bogie.m_curr_segment = segment;
        ret_bogie.m_curr_segment_len = segment->getLength();
      } else {
        // Конец пути достигнут
        ret_bogie.m_curr_segment = nullptr;
        ret_bogie.m_t_coordinate = -1.f;
        ret_bogie.m_curr_segment_len = 0;
      }
    }
  } else {
    // Первая тележка движется назад
    while (ret_bogie.m_t_coordinate < 0.f && ret_bogie.m_curr_segment) {
      // Переход на предыдущий сегмент пути
      segment = m_callback_prev_segment_f(segment);

      if (segment) {
        // Перерасчет параметра t для нового сегмента
        ret_bogie.m_t_coordinate = 1 + ret_bogie.m_t_coordinate *
                                           ret_bogie.m_curr_segment_len /
                                           segment->getLength();
        ret_bogie.m_curr_segment = segment;
        ret_bogie.m_curr_segment_len = segment->getLength();
      } else {
        // Попали в тупик
        ret_bogie.m_curr_segment = nullptr;
        ret_bogie.m_t_coordinate = -1.f;
        ret_bogie.m_curr_segment_len = 0;
      }
    }
  }

  return ret_bogie;
}

// SegmentPosition SegmentPosition::calcBySegmentLength(float length) const {
//   SegmentPosition ret_pos(m_curr_segment, m_t_coordinate);
// }

SegmentPosition SegmentPosition::calcByDistance(float distance) const {
  if (!m_curr_segment || m_curr_segment_len < Math::Consts::EPS)
    return SegmentPosition();
  if (Math::abs(distance) < Math::Consts::EPS)
    return SegmentPosition(m_curr_segment, m_t_coordinate);

  SegmentPosition ret_pos(m_curr_segment, m_t_coordinate);

  // Рассчитываем текущие позиции тележек
  Math::Vec3 v_first = getWorldPosition();
  Math::Vec3 v_second = v_first;

  float calc_distance = 0.f;
  float excess = calc_distance - Math::abs(distance);

  int count = 0;

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
        // Возврат к начальному сегменту
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
        // Возврат к начальному сегменту
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
    // Рассчитываем новые позиции тележек
    v_second = ret_pos.m_curr_segment->calcPoint(ret_pos.m_t_coordinate);

    // Рассчитываем новое расстояние между тележками
    calc_distance = (v_first - v_second).length();

    // Рассчитываем разницу в расстоянии
    excess = calc_distance - Math::abs(distance);

    // Увеличение итерации
    ++count;
  }

  return ret_pos;
}

Math::Vec3 SegmentPosition::getWorldPosition() const {
  if (m_curr_segment)
    return m_curr_segment->calcPoint(m_t_coordinate);
  else
    return Math::Vec3_zero;
}

Math::vec3 SegmentPosition::getDirection() const {
  if (m_curr_segment)
    return m_curr_segment->calcTangent(m_t_coordinate);
  else
    return Math::vec3_zero;
}