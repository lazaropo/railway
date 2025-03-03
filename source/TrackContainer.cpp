#include "TrackContainer.h"

/**
 * @brief Регистрирует компонент TrackContrainer в системе компонентов Unigine.
 */
REGISTER_COMPONENT(TrackContrainer);

using namespace Unigine;

/**
 * @brief Инициализация компонента TrackContrainer.
 *
 * Этот метод создает новый граф сплайнов, загружает его из файла и присваивает
 * источники для каждого сегмента сплайна. Если загрузка графа сплайнов
 * неуспешна, программа аварийно завершается.
 */
void TrackContrainer::init() {
  if (m_spline_graph) return;  // Если граф уже существует, выходим

  // Создание нового графа сплайнов
  m_spline_graph = WorldSplineGraph::create();

  // Загрузка графа сплайнов из файла
  m_spline_graph->load(spline_file);

  // Проверка успешности загрузки
  if (!m_spline_graph) {
    // Сообщение об ошибке и завершение программы, если загрузка не удалась
    Log::message("%s hasn't WorldSplineGraph type.\n", getNode()->getName());
    std::terminate();
  }

  // Получение списка сегментов сплайнов
  Vector<SplineSegmentPtr> v_spline_segments;
  m_spline_graph->getSplineSegments(v_spline_segments);

  // Вывод количества сегментов сплайнов
  Log::message("Track container\nSpline segment count: %d\n",
               v_spline_segments.size());

  // Присваивание источников для каждого сегмента сплайна
  for (SplineSegmentPtr& segment : v_spline_segments) {
    segment->assignSource(node_file, SplineSegment::FORWARD_Y);
    segment->setSegmentMode(node_file, SplineSegment::SEGMENT_STRETCH);
  }
}

Vector<Unigine::SplineSegmentPtr> TrackContrainer::getSplineSegments() const {
  Unigine::Vector<Unigine::SplineSegmentPtr> ret;
  m_spline_graph->getSplineSegments(ret);
  return ret;
}
