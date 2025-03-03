#include "Bogie.h"

REGISTER_COMPONENT(Bogie);

using namespace Unigine;

void Bogie::init() {
  //   NodeReferencePtr ref = checked_ptr_cast<NodeReference>(bogie_node.get());
  //   if (ref) {
  //     m_node = ref;
  //   }
}

void Bogie::update() {
  if (m_position.isEmpty()) return;
  Math::Vec3 coordinate = m_position.getWorldPosition();
  Math::vec3 direction = m_position.getDirection();

  getNode()->setWorldPosition(coordinate);
  getNode()->setDirection(direction, Math::vec3_up, Math::AXIS_Y);

  Visualizer::renderVector(
      coordinate,
      coordinate + Math::Vec3(getNode()->getDirection(Math::AXIS_X)),
      Math::vec4_red);
  Visualizer::renderVector(
      coordinate,
      coordinate + Math::Vec3(getNode()->getDirection(Math::AXIS_Y)),
      Math::vec4_green);
  Visualizer::renderVector(
      coordinate,
      coordinate + Math::Vec3(getNode()->getDirection(Math::AXIS_Z)),
      Math::vec4_black);
}

// void Bogie::moveBy(float distance) { m_position =
// m_position.moveBy(distance); }