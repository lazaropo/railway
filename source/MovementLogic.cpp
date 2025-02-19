#include "MovementLogic.h"

using namespace Unigine;

/**
 * @brief Перемещает первую тележку.
 *
 * Этот метод перемещает первую тележку на определенное расстояние вдоль
 * текущего сегмента пути. Если тележка достигает конца сегмента, она переходит
 * на следующий или предыдущий сегмент в зависимости от направления движения.
 *
 * @param first Информация о первой тележке.
 * @param v_speed Скорость движения.
 * @return Новая информация о положении первой тележки.
 */
BogiePos MovementLogic::moveFirstBogie(BogiePos first, float v_speed) {
  // Рассчитываем пройденное расстояние
  float distance = first.m_t_coordinate +
                   v_speed * m_callback_get_ifps_f() / first.m_curr_segment_len;
  BogiePos ret_pos = first;

  SplineSegmentPtr segment = first.m_curr_segment;

  // Если первая тележка движется вперед
  if (distance > 0.f) {
    while (distance - 1.f > Math::Consts::EPS && ret_pos.m_curr_segment) {
      // Переход на следующий сегмент пути
      segment = m_callback_next_segment_f(segment);

      if (segment) {
        // Перерасчет параметра t для нового сегмента
        distance = (distance - 1.f) * ret_pos.m_curr_segment_len /
                   segment->getLength();
        ret_pos.m_curr_segment = segment;
        ret_pos.m_curr_segment_len = segment->getLength();
      } else {
        // Конец пути достигнут
        ret_pos.m_curr_segment = nullptr;
        ret_pos.m_t_coordinate = -1.f;
        ret_pos.m_curr_segment_len = 0;
      }
    }
  } else {
    // Первая тележка движется назад
    while (distance < Math::Consts::EPS && ret_pos.m_curr_segment) {
      // Переход на предыдущий сегмент пути
      segment = m_callback_prev_segment_f(segment);

      if (segment) {
        // Перерасчет параметра t для нового сегмента
        distance =
            1 + distance * ret_pos.m_curr_segment_len / segment->getLength();
        ret_pos.m_curr_segment = segment;
        ret_pos.m_curr_segment_len = segment->getLength();
      } else {
        // Попали в тупик
        ret_pos.m_curr_segment = nullptr;
        ret_pos.m_t_coordinate = -1.f;
        ret_pos.m_curr_segment_len = 0;
      }
    }
  }

  // Обновляем параметр t для новой позиции
  ret_pos.m_t_coordinate = distance;

  return ret_pos;
}

/**
 * @brief Рассчитывает положение второй тележки относительно первой.
 *
 * Этот метод вычисляет новое положение второй тележки таким образом, чтобы
 * расстояние между первой и второй тележкой соответствовало заданному значению
 * - расстоянию между тележками.
 *
 * @param first Информация о первой тележке.
 * @param second Информация о второй тележке.
 * @param distance_btw_bogie Расстояние между тележками.
 * @return Новая информация о положении второй тележки.
 */
BogiePos MovementLogic::calcSecondBogiePos(const BogiePos& first,
                                           BogiePos second,
                                           float distance_btw_bogie) {
  BogiePos ret_pos = second;
  if (!first.m_curr_segment || !second.m_curr_segment) return ret_pos;

  // Рассчитываем текущие позиции тележек
  Math::Vec3 v_first = first.m_curr_segment->calcPoint(first.m_t_coordinate);
  Math::Vec3 v_second = second.m_curr_segment->calcPoint(second.m_t_coordinate);
  float distance = (v_first - v_second).length();
  float excess = distance - Math::abs(distance_btw_bogie);
  float new_t_coordinate = ret_pos.m_t_coordinate;

  int count = 0;

  // Итерируем до тех пор, пока разница в расстоянии не станет незначительной
  // или если число итераций дошло до 10. Это просто эмпирический параметр. Как
  // правило точность расчёта после 10й итерации не увеличивается.
  while (Math::abs(excess) > Math::Consts::EPS && count < 10) {
    if (distance_btw_bogie > 0.f)
      new_t_coordinate += excess / ret_pos.m_curr_segment_len;
    else
      new_t_coordinate -= excess / ret_pos.m_curr_segment_len;

    // Проверяем выход за пределы текущего сегмента пути
    while (new_t_coordinate > 1.f) {
      // Переход на следующий сегмент пути
      ret_pos.m_curr_segment =
          m_callback_next_segment_f(ret_pos.m_curr_segment);

      if (!ret_pos.m_curr_segment) {
        // Возврат к начальному сегменту
        ret_pos.m_curr_segment = first.m_curr_segment;
        new_t_coordinate = 1.f;
      } else {
        // Перерасчет параметра t для нового сегмента
        new_t_coordinate = (new_t_coordinate - 1.f) *
                           ret_pos.m_curr_segment_len /
                           ret_pos.m_curr_segment->getLength();
      }

      // Обновляем длину текущего сегмента
      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    }

    // Проверяем выход за начало текущего сегмента пути
    while (new_t_coordinate < 0.f) {
      // Переход на предыдущий сегмент пути
      ret_pos.m_curr_segment =
          m_callback_prev_segment_f(ret_pos.m_curr_segment);

      if (!ret_pos.m_curr_segment) {
        // Возврат к начальному сегменту
        ret_pos.m_curr_segment = first.m_curr_segment;
        new_t_coordinate = 0.f;
      } else {
        // Перерасчет параметра t для нового сегмента
        new_t_coordinate = 1.f + new_t_coordinate * ret_pos.m_curr_segment_len /
                                     ret_pos.m_curr_segment->getLength();
      }

      // Обновляем длину текущего сегмента
      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    }

    // Рассчитываем новые позиции тележек
    v_second = ret_pos.m_curr_segment->calcPoint(new_t_coordinate);

    // Рассчитываем новое расстояние между тележками
    distance = (v_first - v_second).length();

    // Рассчитываем разницу в расстоянии
    excess = distance - Math::abs(distance_btw_bogie);

    // Увеличение итерации
    ++count;
  }

  // Обновляем параметр t для новой позиции
  ret_pos.m_t_coordinate = new_t_coordinate;

  return ret_pos;
}