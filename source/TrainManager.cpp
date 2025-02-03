#include "TrainManager.h"

REGISTER_COMPONENT(TrainManager);

using namespace Unigine;

void TrainManager::init() {
  m_track_container =
      ComponentSystem::get()->getComponent<TrackContrainer>(track_container);
  if (!m_track_container)
    throw std::runtime_error(
        "There are not any node with assigned TrackContainer property.\n");

  Vector<Unigine::NodePtr> root_nodes;
  World::getRootNodes(root_nodes);
  for (auto& it : root_nodes)
    if (Train* ptr =
            Unigine::ComponentSystem::get()->getComponentInChildren<Train>(it))
      m_trains.push_back(ptr);

  m_segments = m_track_container->getSplineSegments();

  if (m_trains.size()) {
    setNewSegment(m_trains[0], m_segments[643], m_segments);

    Visualizer::renderVector(Math::Vec3(0.0f, 0.0f, 0.1f),
                             Math::Vec3(1.0f, 0.0f, 0.1f), Math::vec4_red);
    Visualizer::renderVector(Math::Vec3(0.0f, 0.0f, 0.1f),
                             Math::Vec3(0.0f, 1.0f, 0.1f), Math::vec4_green);
    Visualizer::renderVector(Math::Vec3(0.0f, 0.0f, 0.1f),
                             Math::Vec3(0.0f, 0.0f, 1.1f), Math::vec4_blue);

    auto pos_node = m_trains[0]->getNode()->getPosition();
    Log::message("Spline point %d with coordinates: x - %f  y - %f  z - %f\n",
                 643, pos_node[0], pos_node[1], pos_node[2]);
  }
}

void TrainManager::update() {
  for (auto train : m_trains) {
    if (!train) continue;

    Train::MOVE ret_value = train->moveNode();
    if (ret_value == Train::MOVE::STOP)
      continue;
    else if (ret_value == Train::MOVE::END)
      setNewSegment(train, m_segments);
    else if (ret_value == Train::MOVE::FAIL)
      throw std::logic_error(
          String::format("Fail at %s move.", train->getNode()->getName()));
  }
}

void TrainManager::setNewSegment(
    Train* train, const Vector<Ptr<SplineSegment>>& v_spline_segments) {
  SplineSegmentPtr curr_segment = train->getNextSegment();
  if (!curr_segment) return;

  SplineSegmentPtr next_segment = nullptr;
  Math::Vec3 curr_end_point = curr_segment->getEndPoint()->getPosition();
  for (auto it : m_segments) {
    if (curr_segment == it ||
        curr_end_point != it->getStartPoint()->getPosition())
      continue;
    else {
      next_segment = it;
      break;
    }
  }

  train->setSegment(curr_segment, next_segment);
}

void TrainManager::setNewSegment(
    Train* train, SplineSegmentPtr curr_segment,
    const Vector<Ptr<SplineSegment>>& v_spline_segments) {
  if (!curr_segment) return;

  auto pos = curr_segment->getStartPoint()->getPosition();
  Log::message("Train start pos: x - %f  y - %f  z - %f\n", pos[0], pos[1],
               pos[2]);

  SplineSegmentPtr next_segment = nullptr;
  Math::Vec3 curr_end_point = curr_segment->getEndPoint()->getPosition();

  Log::message("Spline segment count: %d\n", v_spline_segments.size());
  for (auto it : v_spline_segments) {
    // it->show();
    if (curr_segment == it ||
        curr_end_point != it->getStartPoint()->getPosition())
      continue;
    else {
      next_segment = it;
      break;
    }
  }

  train->setSegment(curr_segment, next_segment);
}
