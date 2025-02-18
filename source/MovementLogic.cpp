#include "MovementLogic.h"

using namespace Unigine;

BogiePos MovementLogic::moveFirstBogie(BogiePos first, float v_speed) {
  float distance = first.m_t_coordinate +
                   v_speed * m_callback_get_ifps_f() / first.m_curr_segment_len;
  BogiePos ret_pos = first;

  SplineSegmentPtr segment = first.m_curr_segment;
  if (distance > 0.f) {
    while (distance - 1.f > Math::Consts::EPS && ret_pos.m_curr_segment) {
      segment = m_callback_next_segment_f(segment);

      if (segment) {
        distance = (distance - 1.f) * ret_pos.m_curr_segment_len /
                   segment->getLength();
        ret_pos.m_curr_segment = segment;
        ret_pos.m_curr_segment_len = segment->getLength();
      } else {
        ret_pos.m_curr_segment = nullptr;
        ret_pos.m_t_coordinate = -1.f;
        ret_pos.m_curr_segment_len = 0;
      }
    }
  } else {
    while (distance < Math::Consts::EPS && ret_pos.m_curr_segment) {
      segment = m_callback_prev_segment_f(segment);

      if (segment) {
        distance =
            1 + distance * ret_pos.m_curr_segment_len / segment->getLength();
        ret_pos.m_curr_segment = segment;
        ret_pos.m_curr_segment_len = segment->getLength();
      } else {
        ret_pos.m_curr_segment = nullptr;
        ret_pos.m_t_coordinate = -1.f;
        ret_pos.m_curr_segment_len = 0;
      }
    }
  }

  ret_pos.m_t_coordinate = distance;

  return ret_pos;
}

BogiePos MovementLogic::calcSecondBogiePos(const BogiePos& first,
                                           BogiePos second,
                                           float distance_btw_bogie) {
  BogiePos ret_pos = second;
  if (!first.m_curr_segment || !second.m_curr_segment) return ret_pos;

  Math::Vec3 v_first = first.m_curr_segment->calcPoint(first.m_t_coordinate);
  Math::Vec3 v_second = second.m_curr_segment->calcPoint(second.m_t_coordinate);
  float distance = (v_first - v_second).length();
  float excess = distance - Math::abs(distance_btw_bogie);
  float new_t_coordinate = ret_pos.m_t_coordinate;

  int count = 0;

  while (Math::abs(excess) > Math::Consts::EPS && count < 10) {
    if (distance_btw_bogie > 0.f)
      new_t_coordinate += excess / ret_pos.m_curr_segment_len;
    else
      new_t_coordinate -= excess / ret_pos.m_curr_segment_len;

    while (new_t_coordinate > 1.f) {
      ret_pos.m_curr_segment =
          m_callback_next_segment_f(ret_pos.m_curr_segment);

      if (!ret_pos.m_curr_segment) {
        ret_pos.m_curr_segment = first.m_curr_segment;  // getCurrSegment()
        new_t_coordinate = 1.f;
      } else
        new_t_coordinate = (new_t_coordinate - 1.f) *
                           ret_pos.m_curr_segment_len /
                           ret_pos.m_curr_segment->getLength();

      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    }
    while (new_t_coordinate < 0.f) {
      ret_pos.m_curr_segment =
          m_callback_prev_segment_f(ret_pos.m_curr_segment);

      if (!ret_pos.m_curr_segment) {
        ret_pos.m_curr_segment = first.m_curr_segment;  // getCurrentSegment();
        new_t_coordinate = 0.f;
      } else
        new_t_coordinate = 1.f + new_t_coordinate * ret_pos.m_curr_segment_len /
                                     ret_pos.m_curr_segment->getLength();

      ret_pos.m_curr_segment_len = ret_pos.m_curr_segment->getLength();
    }

    v_second = ret_pos.m_curr_segment->calcPoint(new_t_coordinate);

    distance = (v_first - v_second).length();

    excess = distance - Math::abs(distance_btw_bogie);

    ++count;
  }

  ret_pos.m_t_coordinate = new_t_coordinate;

  return ret_pos;
}