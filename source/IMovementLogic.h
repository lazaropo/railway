#pragma once

#include <UnigineWorlds.h>

#include <functional>

#include "BogiePos.h"

class IMovementLogic {
 public:
  virtual BogiePos moveFirstBogie(BogiePos first, float v_speed) = 0;
  virtual BogiePos calcSecondBogiePos(const BogiePos& first, BogiePos second,
                                      float distance_btw_bogie) = 0;

  /**
   * @brief Set the callback function for getting the previous segment.
   * @param fp Function pointer to the callback function.
   */
  void setPrevSegmentFunction(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
    m_callback_prev_segment_f = fp;
  }

  /**
   * @brief Set the callback function for getting the next segment.
   * @param fp Function pointer to the callback function.
   */
  void setNextSegmentFunction(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
    m_callback_next_segment_f = fp;
  }

  /**
   * @brief Set the callback function for getting the ifps.
   * @param fp Function pointer to the callback function.
   */
  void setGetIfpsFunction(std::function<float()> fp) {
    m_callback_get_ifps_f = fp;
  }

  //   void setBogieDistance(float distance) {
  //     m_bogie_distance = distance;
  //   }

 protected:
  /**
   * @brief Callback function for getting the previous segment.
   */
  inline static std::function<Unigine::SplineSegmentPtr(
      Unigine::SplineSegmentPtr)>
      m_callback_prev_segment_f = nullptr;

  /**
   * @brief Callback function for getting the next segment.
   */
  inline static std::function<Unigine::SplineSegmentPtr(
      Unigine::SplineSegmentPtr)>
      m_callback_next_segment_f = nullptr;

  /**
   * @brief Callback function for getting the ifps from tarin container.
   */
  inline static std::function<float()> m_callback_get_ifps_f = nullptr;

  // float m_bogie_distance = 0.f;
};
