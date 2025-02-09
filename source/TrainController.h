#pragma once

#include <UnigineComponentSystem.h>

#include <functional>

#include "Train.h"
// #include "TrainManager.h"

class TrainController : public Train {
 public:
  COMPONENT_DEFINE(TrainController, Train);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  void moveTrain() override;

  void setNextSegmentFunction(
      std::function<Unigine::SplineSegmentPtr(Unigine::SplineSegmentPtr)> fp) {
    m_callback_next_segment_f = fp;
  }

 private:
  // void moveBogie(Unigine::NodePtr bogie, float ifps);
  void setCarBody(Unigine::NodePtr body);

  Unigine::Math::Mat4 makeBogieTransform(const Unigine::NodePtr node,
                                         BogiePos pos, float ifps);

  Unigine::Math::Mat4 calcNewPosition();

 protected:
  void init();
  void update();

 private:
  inline static std::function<Unigine::SplineSegmentPtr(
      Unigine::SplineSegmentPtr)>
      m_callback_next_segment_f;
};
