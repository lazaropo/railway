#include "TrackContainer.h"

REGISTER_COMPONENT(TrackContrainer);

using namespace Unigine;

void TrackContrainer::init() {
  if (m_spline_graph) return;

  m_spline_graph = WorldSplineGraph::create();
  m_spline_graph->load(spline_file);

  if (!m_spline_graph) {
    Log::message("%s hasn\'t WorldSplineGraph type.\n", getNode()->getName());
    std::terminate();
  }

  Vector<SplineSegmentPtr> v_spline_segments;
  m_spline_graph->getSplineSegments(v_spline_segments);

  Log::message("Track container\nSpline segment count: %d\n",
               v_spline_segments.size());

  for (SplineSegmentPtr& segment : v_spline_segments) {
    segment->assignSource(node_file, SplineSegment::FORWARD_Y);
    segment->setSegmentMode(node_file, SplineSegment::SEGMENT_STRETCH);
  }
}
