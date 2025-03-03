#pragma once

#include <UnigineComponentSystem.h>

#include "Carriage.h"

class Train : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(Train, Unigine::ComponentBase);

  PROP_PARAM(Float, start_speed, 10.f);
  PROP_PARAM(Float, max_speed, 40.f);
  PROP_PARAM(Float, margin, 1.f);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  void moveBy();

  void setStartPosition(Unigine::SplineSegmentPtr segment,
                        float t_coordinate = 0);

 protected:
  void init();
  void update();

  bool m_is_stop = false;
  float m_speed = start_speed;

  Unigine::Vector<Carriage*> m_carriage;
  float m_carraige_len;

  SegmentPosition m_position;
};
