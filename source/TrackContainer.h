#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineConsole.h>
#include <UnigineMathLib.h>
#include <UnigineWorlds.h>

class TrackContrainer : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(TrackContrainer, ComponentBase);

  PROP_PARAM(File, spline_file, NULL);
  PROP_PARAM(File, node_file, NULL);

  COMPONENT_INIT(init);

  void init();

  Unigine::Vector<Unigine::SplineSegmentPtr> getSplineSegments() const {
    Unigine::Vector<Unigine::SplineSegmentPtr> ret;
    m_spline_graph->getSplineSegments(ret);
    return ret;
  }

  Unigine::Math::Vec4 makeNextPosition(const Unigine::Math::Vec4& current_pos,
                                       const Unigine::Math::Vec3& direction,
                                       float speed);

 private:
  Unigine::WorldSplineGraphPtr m_spline_graph = nullptr;
};
