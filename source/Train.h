#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <UniginePrimitives.h>  // Box primitive - m_car_body
#include <UnigineVisualizer.h>
#include <UnigineWorlds.h>

class Train : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(Train, Unigine::ComponentBase);

  PROP_PARAM(Float, speed, 30);

  enum MOVE {
    SUCCESS,  // node is moved
    END,      // end of current segment
    STOP,     // end of this segments path
    FAIL      // fails at move function
  };

  void setSegment(Unigine::SplineSegmentPtr curr_segment,
                  Unigine::SplineSegmentPtr next_segment);
  Unigine::SplineSegmentPtr getNextSegment() { return m_next_segment; }

  MOVE moveNode();

 private:
  void moveNode(const Unigine::Math::Vec3& pos,
                const Unigine::Math::vec3& angle);

 private:
  inline static int m_count = 0;
  Unigine::ObjectMeshDynamicPtr m_forward_bogey;
  Unigine::ObjectMeshDynamicPtr m_back_bogey;
  Unigine::ObjectMeshDynamicPtr m_car_body;

  float m_t_coordinate = 0;
  Unigine::SplineSegmentPtr m_curr_segment;
  Unigine::SplineSegmentPtr m_next_segment;

  float m_curr_segment_len = 0;
  float m_excess_len = 0;
};
