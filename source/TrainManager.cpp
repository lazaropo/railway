#include "TrainManager.h"

REGISTER_COMPONENT(TrainManager);

using namespace Unigine;

TrainManager* TrainManager::m_instance = nullptr;

void TrainManager::init() {
  if (!m_instance)
    m_instance = this;
  else
    return;

  m_track_container =
      ComponentSystem::get()->getComponent<TrackContrainer>(track_container);
  if (!m_track_container)
    throw std::runtime_error(
        "There are not any node with assigned TrackContainer property.\n");

  // if (m_carriage.size() && m_carriage[0]->getNode()) {
  //   //
  //   m_carriage[0]->setSplineSegment(m_track_container->getSplineSegments()[83]);

  //   auto pos_node = m_carriage[0]->getNode()->getPosition();
  //   // Log::message("Spline point %d with coordinates: x - %f  y - %f  z -
  //   // %f\n",
  //   //              86, pos_node[0], pos_node[1], pos_node[2]);
  // }
}

void TrainManager::setCarriage(Carriage* carriage) {
  if (carriage) TrainManager::getInstance()->m_carriage.push_back(carriage);
}

Unigine::SplineSegmentPtr TrainManager::getNextSegment(
    Unigine::SplineSegmentPtr curr_segment) {
  if (!curr_segment) return nullptr;

  SplineSegmentPtr next_segment = nullptr;
  Math::Vec3 curr_end_point = curr_segment->getEndPoint()->getPosition();

  bool is_current_exists = false;

  for (auto it :
       TrainManager::getInstance()->m_track_container->getSplineSegments()) {
    if (curr_segment == it) {
      is_current_exists = true;
      continue;
    }
    if ((curr_end_point - it->getStartPoint()->getPosition()).length() <
        Math::Consts::EPS) {
      next_segment = it;
      break;
    }
  }
  // if (!is_current_exists)
  //   throw std::logic_error("Spline graph doesnt contains current
  //   segment.\n");

  return next_segment;
}

Unigine::SplineSegmentPtr TrainManager::getPrevSegment(
    Unigine::SplineSegmentPtr curr_segment) {
  if (!curr_segment) return nullptr;

  SplineSegmentPtr prev_segment = nullptr;
  Math::Vec3 curr_start_point = curr_segment->getStartPoint()->getPosition();

  bool is_current_exists = false;

  for (auto it :
       TrainManager::getInstance()->m_track_container->getSplineSegments()) {
    if (curr_segment == it) {
      is_current_exists = true;
      continue;
    }
    if ((curr_start_point - it->getEndPoint()->getPosition()).length() <
        Math::Consts::EPS) {
      prev_segment = it;
      break;
    }
    // if (!prev_segment && it == *TrainManager::getInstance()
    //                                 ->m_track_container->getSplineSegments()
    //                                 .back())
    //   prev_segment = nullptr;
  }

  // if (!is_current_exists)
  //   throw std::logic_error("Spline graph doesnt contains current
  //   segment.\n");

  // if (!prev_segment) return nullptr;

  return prev_segment;
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
