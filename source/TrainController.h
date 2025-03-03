/**
 * @file TrainController.hpp
 * @brief Определение класса TrainController.
 *
 * Этот файл содержит определение класса TrainController, который управляет
 * движением поезда в игровом движке Unigine. Классы контролирует перемещение
 * поезда вдоль заданного пути, управление скоростью и изменение направления
 * движения.
 */

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>
#include <UniginePrimitives.h>  // Box primitive - m_car_node
#include <UnigineVisualizer.h>
// #include <UnigineNodeReference.h>
#include <UnigineWorld.h>
#include <UnigineWorlds.h>

#include <memory>
#include <stdexcept>

#include "BogiePos.h"
#include "IMovementLogic.h"

/**
 * @class TrainController
 * @brief Компонент для управления поездом в игровом движке Unigine.
 *
 * Этот класс представляет собой компонент, управляющий движением поезда вдоль
 * определенного пути. Он отвечает за перемещение поезда, управление его
 * скоростью и изменением направления движения.
 */
class TrainController : public Unigine::ComponentBase {
 public:
  /**
   * @brief Определяем тип компонента как TrainController.
   */
  COMPONENT_DEFINE(TrainController, Unigine::ComponentBase);

  /**
   * @brief Параметр для начальной скорости поезда.
   */
  PROP_PARAM(Float, start_speed, 20.f);

  /**
   * @brief Параметр для максимальной допустимой скорости поезда.
   */
  PROP_PARAM(Float, max_speed, 40.f);

  /**
   * @enum MOVE_DIRECTION
   * @brief Направление движения поезда.
   */
  enum MOVE_DIRECTION {
    /** Движение вперед. */
    FORWARD,
    /** Движение назад. */
    REVERSE
  };

  /**
   * @brief Инициализируем компонент.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Обновляем состояние компонента на каждом кадре.
   */
  COMPONENT_UPDATE(update);

  /**
   * @brief Устанавливаем логику движения поезда.
   * @param logic Указатель на полиморфный объект логики движения.
   */
  void setMovementLogic(std::shared_ptr<IMovementLogic> logic) {
    m_movement_logic = logic;
  }

  /**
   * @brief Устанавливаем начальный сегмент пути и позицию на нем.
   * Используем этот метод один раз от класса Carriage для установки поездов.
   * @param curr_segment Текущий сегмент сплайна.
   * @param prev_segment Предыдущий сегмент сплайна.
   * @param pos Позиция на сегменте (по умолчанию: 0).
   */
  void setTrainAtSegment(Unigine::SplineSegmentPtr curr_segment,
                         Unigine::SplineSegmentPtr prev_segment,
                         float pos = 0.f);

  /**
   * @brief Устанавливаем функцию обратного вызова для начала движения всего
   * состава.
   * @param fp Функция обратного вызова.
   */
  void setMoveStartFunc(std::function<void()> fp) {
    m_callback_move_start = fp;
  }

  /**
   * @brief Устанавливаем функцию обратного вызова для остановки движения всего
   * состава.
   * @param fp Функция обратного вызова.
   */
  void setMoveEndFunc(std::function<void()> fp) { m_callback_move_stop = fp; }

  /**
   * @brief Останавливаем движение данного поезда.
   */
  void stopMove() { m_is_stop = true; }

  /**
   * @brief Начинаем движение данного поезда.
   */
  void startMove() { m_is_stop = false; }

  /**
   * @brief Ускоряем поезд до достижения максимальной скорости.
   */
  void accelerate() {
    m_new_linear_velocity = Unigine::Math::clamp(
        m_new_linear_velocity + m_acceleration, 0.f, max_speed);
  }

  /**
   * @brief Замедляем движение поезда до полной остановки.
   */
  void brake() {
    m_new_linear_velocity = Unigine::Math::clamp(
        m_new_linear_velocity - m_acceleration, 0.f, max_speed);
  }

  /**
   * @brief Получаем текущее направление движения.
   * @return Текущее направление движения (FORWARD или REVERSE).
   */
  MOVE_DIRECTION getMoveDirection() { return m_current_move_direction; }

  /**
   * @brief Меняем направление движения на противоположное.
   */
  void changeMoveDirection();

  /**
   * @brief Получаем длину поезда.
   * @return Общая длина поезда. На данынй момент равна расстоянию между
   * тележками + зазор TRAINS_MARGIN.
   */
  float getLength() const;

  /**
   * @brief Получаем текущий сегмент пути в зависимости от направления движения.
   * @return Указатель на текущий сегмент пути. При прямом движении возвращаем
   * переднюю тележку, при обратном - заднюю.
   */
  Unigine::SplineSegmentPtr getCurrentSegment() const;

  /**
   * @brief Получаем текущую параметризированную позицию на сегменте пути.
   * @return Текущая параметризированная координата t на сегменте пути.
   */
  float getCurrentParamPos() const;

  /**
   * @brief Получаем информацию о положении передней тележки.
   * @return Структура BogiePos для передней тележки.
   */
  const BogiePos getFBogiePos() const { return m_forward_bogie_pos; }

  /**
   * @brief Получаем информацию о положении задней тележки.
   * @return Структура BogiePos для задней тележки.
   */
  const BogiePos getBBogiePos() const { return m_back_bogie_pos; }

 protected:
  /**
   * @brief Перемещаем поезд вдоль пути.
   */
  void moveTrain();

  /**
   * @brief Отображение осей локальной системы координат для данного узла через
   * Visualizer.
   * @param node Узел, который необходимо отрендерить.
   */
  void renderNode(Unigine::NodePtr node);

 private:
  /**
   * @brief Устанавливаем текущий сегмент для движения вперед. Используется из
   * @ref setSegment.
   * @param curr_segment Сегмент, который необходимо установить.
   * @param prev_segment Предыдущий сегмент.
   * @param pos Позиция на сегменте (по умолчанию: 0).
   */
  void setSegmentForward(Unigine::SplineSegmentPtr curr_segment,
                         Unigine::SplineSegmentPtr prev_segment,
                         float pos = 0.f);

  /**
   * @brief Устанавливаем текущий сегмент для движения назад. Используется из
   * @ref setSegment.
   * @param curr_segment Сегмент, который необходимо установить.
   * @param prev_segment Предыдущий сегмент.
   * @param pos Позиция на сегменте (по умолчанию: 0).
   */
  void setSegmentReverse(Unigine::SplineSegmentPtr curr_segment,
                         Unigine::SplineSegmentPtr prev_segment,
                         float pos = 0.f);

  /**
   * @brief Внутренний метод инициализации.
   */
  void init();

  /**
   * @brief Внутренний метод обновления, вызываемый на каждом кадре.
   */
  void update();

 protected:
  /**
   * @brief Расстояние между поездами.
   */
  static constexpr float TRAINS_MARGIN = 1.f;

  /**
   * @brief Статический счетчик экземпляров этого компонента.
   */
  inline static int m_count = 0;

  /**
   * @brief Текущая линейная скорость поезда.
   */
  float m_current_linear_velocity = start_speed;

  /**
   * @brief Новая линейная скорость поезда.
   */
  float m_new_linear_velocity = start_speed;

  /**
   * @brief Величина ускорения поезда.
   */
  float m_acceleration = 1.f;

  /**
   * @brief Флаг, указывающий, остановлен ли поезд.
   */
  bool m_is_stop = false;

  /**
   * @brief Расстояние между двумя тележками.
   */
  float m_bogie_distance = 0.f;

  /**
   * @brief Текущее направление движения.
   */
  MOVE_DIRECTION m_current_move_direction = MOVE_DIRECTION::FORWARD;

  /**
   * @brief Предыдущий сегмент пути.
   */
  Unigine::SplineSegmentPtr m_prev_segment;

  /**
   * @brief Узел передней тележки.
   */
  Unigine::NodePtr m_forward_bogie_node;

  /**
   * @brief Узел задней тележки.
   */
  Unigine::NodePtr m_back_bogie_node;

  /**
   * @brief Узел кузова поезда.
   */
  Unigine::NodePtr m_car_node;

  /**
   * @brief Информация о положении передней тележки.
   */
  BogiePos m_forward_bogie_pos;

  /**
   * @brief Информация о положении задней тележки.
   */
  BogiePos m_back_bogie_pos;

  /**
   * @brief Функция обратного вызова, вызываемая для запуска движения состава.
   */
  std::function<void()> m_callback_move_start;

  /**
   * @brief Функция обратного вызова, вызываемая для остановки движения состава.
   */
  std::function<void()> m_callback_move_stop;
};
