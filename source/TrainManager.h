#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>
#include <UnigineWorld.h>

#include <stdexcept>

// #include "InputProcessingBase.h"
#include "TrackContainer.h"
#include "Train.h"

class TrainManager : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(TrainManager, Unigine::ComponentBase);

  PROP_PARAM(Node, track_container, nullptr);

  COMPONENT_INIT(init);
  // COMPONENT_UPDATE(update);

  void init();
  // void update();

  void setTrain(Train* train);

  Unigine::SplineSegmentPtr getNextSegment(
      Unigine::SplineSegmentPtr prev_segment);

  //   void setNewSegment(
  //       Train* train,
  //       const Unigine::Vector<Unigine::SplineSegmentPtr>& v_spline_segments);
  //   void setNewSegment(
  //       Train* train, Unigine::SplineSegmentPtr curr_segment,
  //       const Unigine::Vector<Unigine::SplineSegmentPtr>& v_spline_segments);

 private:
  TrackContrainer* m_track_container = nullptr;

  Unigine::Vector<Train*> m_trains;
  // InputProcessingBase* m_input_strategy = nullptr;
};
