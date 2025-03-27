#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>
#include <UnigineWorld.h>

#include <stdexcept>

#include "TrackContainer.h"
#include "Train.h"

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
   *
   * Этот макрос определяет компонент TrainManager, наследующий от
   * Unigine::ComponentBase.
   */
  COMPONENT_DEFINE(TrainManager, Unigine::ComponentBase);

  /**
   * @brief Массив с нодами поездов.
   *
   * Параметр хранит ноды с компонентами Train.
   */
  PROP_ARRAY(Node, train_container);

  /**
   * @brief Параметр для компонента со сплайн графом. Предполагается
   * TrackContainer.
   */
  PROP_PARAM(Node, track_container, nullptr);

  /**
   * @brief Инициализация компонента.
   *
   * Этот макрос определяет функцию init() как функцию инициализации
   * компонента.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Инициализирующий метод компонента.
   *
   * Эта функция вызывается при инициализации компонента.
   */
  void init();

  /**
   * @brief Возвращает указатель на единственный экземпляр менеджера поездов.
   *
   * @return Указатель на менеджер поездов.
   */
  inline static TrainManager* getInstance() { return m_instance; }

  /**
   * @brief Устанавливает состав, связанный с менеджером поездов.
   *
   * @param train Указатель на состав.
   */
  static void setTrain(Train* train);

  /**
   * @brief Возвращает начальный сегмент пути.
   *
   * @return Указатель на начальный сегмент пути.
   */
  static Unigine::SplineSegmentPtr getStartSegment();

  /**
   * @brief Возвращает следующий сегмент пути относительно указанного.
   *
   * @param curr_segment Указатель на текущий сегмент пути.
   * @return Указатель на следующий сегмент пути.
   */
  static Unigine::SplineSegmentPtr getNextSegment(
      Unigine::SplineSegmentPtr curr_segment);

  /**
   * @brief Возвращает предыдущий сегмент пути относительно указанного.
   *
   * @param curr_segment Указатель на текущий сегмент пути.
   * @return Указатель на предыдущий сегмент пути.
   */
  static Unigine::SplineSegmentPtr getPrevSegment(
      Unigine::SplineSegmentPtr curr_segment);

 private:
  /**
   * @brief Указатель на контейнер путей - сплайнов.
   *
   * Этот указатель хранит ссылку на контейнер путей, представляющий собой
   * сплайн-граф.
   */
  TrackContrainer* m_track_container = nullptr;
};
