#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>
#include <UnigineWorld.h>

#include <stdexcept>

#include "TrackContainer.h"
#include "Train.h"

class TrainManager : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(TrainManager, Unigine::ComponentBase);

  PROP_PARAM(Node, track_container, nullptr);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  void init();
  void update();

  void setNewSegment(
      Train* train,
      const Unigine::Vector<Unigine::SplineSegmentPtr>& v_spline_segments);
  void setNewSegment(
      Train* train, Unigine::SplineSegmentPtr curr_segment,
      const Unigine::Vector<Unigine::SplineSegmentPtr>& v_spline_segments);

 private:
  TrackContrainer* m_track_container = nullptr;
  Unigine::WorldSplineGraphPtr m_graph_ptr = nullptr;
  Unigine::Vector<Unigine::SplineSegmentPtr> m_segments;
  Unigine::Vector<Train*> m_trains;
};
