#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>

#include "SegmentPosition.h"

class Bogie : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(Bogie, Unigine::ComponentBase);

  // PROP_PARAM(Node, bogie_node);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  Unigine::Math::Vec3 getWorldPosition() const {
    return m_position.getWorldPosition();
  }

  SegmentPosition getSegmentPosition() const { return m_position; }
  void setSegmentPosition(SegmentPosition pos) { m_position = pos; }

 protected:
  void init();
  void update();

 public:
  SegmentPosition m_position;

  //  protected:
  //   Unigine::NodePtr m_node;
};
