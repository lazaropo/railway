#pragma once

#include <UnigineWorlds.h>

/**
 * @struct BogiePos
 * @brief Structure to store information about the position of the train's
 * bogies.
 */
struct BogiePos {
  /**
   * @brief Current spline segment.
   */
  Unigine::SplineSegmentPtr m_curr_segment;

  /**
   * @brief Current parametric t-coordinate on the segment.
   */
  float m_t_coordinate = 0;

  /**
   * @brief Length of the current segment.
   */
  float m_curr_segment_len = 0;
};