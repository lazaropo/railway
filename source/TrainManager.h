#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>
#include <UnigineWorld.h>

#include <stdexcept>

// #include "InputProcessingBase.h"
#include "Carriage.h"
#include "TrackContainer.h"

class TrainManager : public Unigine::ComponentBase {
 private:
  static TrainManager* m_instance;

 public:
  COMPONENT_DEFINE(TrainManager, Unigine::ComponentBase);

  PROP_PARAM(Node, track_container, nullptr);

  COMPONENT_INIT(init);

  void init();
  // void update();
  inline static TrainManager* getInstance() { return m_instance; }

  static void setCarriage(Carriage* carriage);

  inline static Unigine::SplineSegmentPtr getStartSegment() {
    return TrainManager::getInstance()
        ->m_track_container->getSplineSegments()[80];
  }

  static Unigine::SplineSegmentPtr getNextSegment(
      Unigine::SplineSegmentPtr curr_segment);

  static Unigine::SplineSegmentPtr getPrevSegment(
      Unigine::SplineSegmentPtr curr_segment);

 private:
  TrackContrainer* m_track_container = nullptr;

  Unigine::Vector<Carriage*> m_carriage;
};
