#include "TrainManager.h"

REGISTER_COMPONENT(TrainManager);

using namespace Unigine;

void TrainManager::init() {
  m_track_container =
      ComponentSystem::get()->getComponent<TrackContrainer>(track_container);
  if (!m_track_container)
    throw std::runtime_error(
        "There are not any node with assigned TrackContainer property.\n");

  // m_trains.push_back(ptr);

  // if (m_trains.size()) {
  //   setNewSegment(m_trains[0], m_track_container->getSplineSegments().at(86),
  //   m_segments);

  auto pos_node = m_trains[0]->getNode()->getPosition();
  Log::message("Spline point %d with coordinates: x - %f  y - %f  z - %f\n", 86,
               pos_node[0], pos_node[1], pos_node[2]);
}

// void TrainManager::update() {
//   for (auto train : m_trains) {
//     if (!train) continue;

//     if(InputController::getInstance()->isAction())
//     m_input_strategy->processInput();

//     Train::MOVE ret_value = train->moveNode();
//     if (ret_value == Train::MOVE::STOP)
//       continue;
//     else if (ret_value == Train::MOVE::END)
//       setNewSegment(train, m_segments);
//     else if (ret_value == Train::MOVE::FAIL)
//       throw std::logic_error(
//           String::format("Fail at %s move.", train->getNode()->getName()));
//   }
// }

void TrainManager::setTrain(Train* train) {
  if (train) m_trains.push_back(train);
}

Unigine::SplineSegmentPtr TrainManager::getNextSegment(
    Unigine::SplineSegmentPtr prev_segment) {
  if (!prev_segment) return nullptr;

  auto pos = prev_segment->getStartPoint()->getPosition();
  Log::message("Train start pos: x - %f  y - %f  z - %f\n", pos[0], pos[1],
               pos[2]);

  SplineSegmentPtr next_segment = nullptr;
  Math::Vec3 prev_end_point = prev_segment->getEndPoint()->getPosition();

  // Log::message("Spline segment count: %d\n", m_track_container->size());
  for (auto it : m_track_container->getSplineSegments()) {
    // it->show();
    if (prev_segment == it ||
        prev_end_point != it->getStartPoint()->getPosition())
      continue;
    else {
      next_segment = it;
      break;
    }
  }

  return next_segment;
}

// void TrainManager::setNewSegment(
//     Train* train, const Vector<Ptr<SplineSegment>>& v_spline_segments) {
//   SplineSegmentPtr curr_segment = train->getNextSegment();
//   if (!curr_segment) return;

//   SplineSegmentPtr next_segment = nullptr;
//   Math::Vec3 curr_end_point = curr_segment->getEndPoint()->getPosition();
//   for (auto it : m_track_container->getSplineSegments()) {
//     if (curr_segment == it ||
//         curr_end_point != it->getStartPoint()->getPosition())
//       continue;
//     else {
//       next_segment = it;
//       break;
//     }
//   }

//   train->setSegment(curr_segment, next_segment);
// }

// void TrainManager::setNewSegment(
//     Train* train, SplineSegmentPtr curr_segment,
//     const Vector<Ptr<SplineSegment>>& v_spline_segments) {
//   if (!curr_segment) return;

//   auto pos = curr_segment->getStartPoint()->getPosition();
//   Log::message("Train start pos: x - %f  y - %f  z - %f\n", pos[0], pos[1],
//                pos[2]);

//   SplineSegmentPtr next_segment = nullptr;
//   Math::Vec3 curr_end_point = curr_segment->getEndPoint()->getPosition();

//   Log::message("Spline segment count: %d\n", v_spline_segments.size());
//   for (auto it : v_spline_segments) {
//     // it->show();
//     if (curr_segment == it ||
//         curr_end_point != it->getStartPoint()->getPosition())
//       continue;
//     else {
//       next_segment = it;
//       break;
//     }
//   }

//   train->setSegment(curr_segment, next_segment);
// }
