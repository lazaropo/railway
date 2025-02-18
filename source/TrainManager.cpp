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
}

void TrainManager::setCarriage(Carriage* carriage) {
  if (carriage) TrainManager::getInstance()->m_carriage.push_back(carriage);
}

Unigine::SplineSegmentPtr TrainManager::getNextSegment(
    Unigine::SplineSegmentPtr curr_segment) {
  if (!curr_segment) return nullptr;

  SplineSegmentPtr next_segment = nullptr;
  Math::Vec3 curr_end_point = curr_segment->getEndPoint()->getPosition();

  for (auto it :
       TrainManager::getInstance()->m_track_container->getSplineSegments()) {
    if (curr_segment == it) continue;

    if ((curr_end_point - it->getStartPoint()->getPosition()).length() <
        Math::Consts::EPS) {
      next_segment = it;
      break;
    }
  }

  return next_segment;
}

Unigine::SplineSegmentPtr TrainManager::getPrevSegment(
    Unigine::SplineSegmentPtr curr_segment) {
  if (!curr_segment) return nullptr;

  SplineSegmentPtr prev_segment = nullptr;
  Math::Vec3 curr_start_point = curr_segment->getStartPoint()->getPosition();

  // bool is_current_exists = false;

  for (auto it :
       TrainManager::getInstance()->m_track_container->getSplineSegments()) {
    if (curr_segment == it) {
      // is_current_exists = true;
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
