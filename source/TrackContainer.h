#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineConsole.h>
#include <UnigineMathLib.h>
#include <UnigineWorlds.h>

/**
 * @class TrackContrainer
 * @brief Компонент для работы с графом путей.
 *
 * Этот компонент управляет графом путей, состоящим из сегментов сплайнов. Он
 * позволяет загружать файлы с описанием путей и получать список сегментов
 * сплайнов.
 */
class TrackContrainer : public Unigine::ComponentBase {
 public:
  /**
   * @brief Определение типа компонента как TrackContrainer.
   */
  COMPONENT_DEFINE(TrackContrainer, ComponentBase);

  /**
   * @brief Параметр для файла с описанием сплайнов.
   */
  PROP_PARAM(File, spline_file, NULL);

  /**
   * @brief Параметр для файла с описанием узлов.
   */
  PROP_PARAM(File, node_file, NULL);

  /**
   * @brief Инициализация компонента.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Инициализирующий метод компонента.
   */
  void init();

  /**
   * @brief Возвращает вектор указателей на сегменты сплайнов.
   *
   * Этот метод возвращает список всех сегментов сплайнов, составляющих путь.
   *
   * @return Вектор указателей на сегменты сплайнов.
   */
  Unigine::Vector<Unigine::SplineSegmentPtr> getSplineSegments() const;

 private:
  /**
   * @brief Указатель на граф сплайнов.
   */
  Unigine::WorldSplineGraphPtr m_spline_graph = nullptr;
};