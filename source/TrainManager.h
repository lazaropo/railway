#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>
#include <UnigineWorld.h>

#include <stdexcept>

#include "Carriage.h"
#include "TrackContainer.h"

/**
 * @class TrainManager
 * @brief Менеджер поездов, управляющий движением и путями. Одиночка.
 *
 * Этот класс управляет составами поездов, обеспечивает взаимодействие с сплайн
 * графом и предоставляет доступ к начальной точке маршрута.
 */
class TrainManager : public Unigine::ComponentBase {
 private:
  /**
   * @brief Указатель на единственный экземпляр менеджера поездов.
   */
  static TrainManager* m_instance;

 public:
  /**
   * @brief Определение типа компонента как TrainManager.
   */
  COMPONENT_DEFINE(TrainManager, Unigine::ComponentBase);

  /**
   * @brief Параметр для компонента со сплайн графом. Предполагается
   * TrackContainer.
   */
  PROP_PARAM(Node, track_container, nullptr);

  /**
   * @brief Инициализация компонента.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Инициализирующий метод компонента.
   */
  void init();

  /**
   * @brief Возвращает указатель на единственный экземпляр менеджера поездов.
   * @return Указатель на менеджер поездов.
   */
  inline static TrainManager* getInstance() { return m_instance; }

  /**
   * @brief Устанавливает состав, связанный с менеджером поездов.
   * @param carriage Указатель на состав.
   */
  static void setCarriage(Carriage* carriage);

  /**
   * @brief Возвращает начальный сегмент пути.
   * @return Указатель на начальный сегмент пути.
   */
  inline static Unigine::SplineSegmentPtr getStartSegment() {
    return TrainManager::getInstance()
        ->m_track_container->getSplineSegments()[80];
  }

  /**
   * @brief Возвращает следующий сегмент пути относительно указанного.
   * @param curr_segment Указатель на текущий сегмент пути.
   * @return Указатель на следующий сегмент пути.
   */
  static Unigine::SplineSegmentPtr getNextSegment(
      Unigine::SplineSegmentPtr curr_segment);

  /**
   * @brief Возвращает предыдущий сегмент пути относительно указанного.
   * @param curr_segment Указатель на текущий сегмент пути.
   * @return Указатель на предыдущий сегмент пути.
   */
  static Unigine::SplineSegmentPtr getPrevSegment(
      Unigine::SplineSegmentPtr curr_segment);

 private:
  /**
   * @brief Указатель на контейнер путей - сплайнов.
   */
  TrackContrainer* m_track_container = nullptr;

  /**
   * @brief Вектор составов, управляемых менеджером поездов.
   */
  Unigine::Vector<Carriage*> m_carriage;
};