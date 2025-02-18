
/**
 * @file TrainController.hpp
 * @brief Definition of the TrainController class.
 */

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>
#include <UniginePrimitives.h>  // Box primitive - m_car_node
#include <UnigineVisualizer.h>
#include <UnigineWorlds.h>

// #include <functional>
#include <memory>

#include "BogiePos.h"
#include "IMovementLogic.h"

/**
 * @class TrainController
 * @brief A component for controlling a train in the Unigine game engine.
 *
 * This class represents a component that controls the movement of a train along
 * a specified path, manages its speed, and changes direction.
 */
class TrainController : public Unigine::ComponentBase {
 public:
  /**
   * @brief Define the component type as TrainController.
   */
  COMPONENT_DEFINE(TrainController, Unigine::ComponentBase);

  /**
   * @brief Parameter for initial speed of the train.
   */
  PROP_PARAM(Float, start_speed, 20.f);

  /**
   * @brief Parameter for maximum allowed speed of the train.
   */
  PROP_PARAM(Float, max_speed, 40.f);

  /**
   * @enum MOVE_DIRECTION
   * @brief Direction of the train's movement.
   */
  enum MOVE_DIRECTION {
    /** Moving forward. */
    FORWARD,
    /** Moving backward. */
    REVERSE
  };

  /**
   * @brief Initialize the component.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Update the component each frame.
   */
  COMPONENT_UPDATE(update);

  void setMovementLogic(std::shared_ptr<IMovementLogic> logic) {
    m_movement_logic = logic;
  }
  /**
   * @brief Set the current start segment and position on it. Use once from
   * @class Carriage to rate the trains.
   * @param curr_segment The current spline segment.
   * @param pos Position on the segment (default: 0).
   */
  void setTrainAtSegment(Unigine::SplineSegmentPtr curr_segment,
                         Unigine::SplineSegmentPtr prev_segment,
                         float pos = 0.f);

  /**
   * @brief Set the callback function for starting the movement.
   * @param fp Function pointer to the callback function.
   */
  void setMoveStartFunc(std::function<void()> fp) {
    m_callback_move_start = fp;
  }

  /**
   * @brief Set the callback function for stopping the movement.
   * @param fp Function pointer to the callback function.
   */
  void setMoveEndFunc(std::function<void()> fp) { m_callback_move_stop = fp; }

  /**
   * @brief Stop the train's movement.
   */
  void stopMove() { m_is_stop = true; }

  /**
   * @brief Start the train's movement.
   */
  void startMove() { m_is_stop = false; }

  /**
   * @brief Increase the train's speed until reaching the maximum value.
   */
  void accelerate() {
    m_new_linear_velocity = Unigine::Math::clamp(
        m_new_linear_velocity + m_acceleration, 0.f, max_speed);
  }

  /**
   * @brief Decrease the train's speed until stopping.
   */
  void brake() {
    m_new_linear_velocity = Unigine::Math::clamp(
        m_new_linear_velocity - m_acceleration, 0.f, max_speed);
  }

  /**
   * @brief Get the current direction of movement.
   * @return The current MOVE_DIRECTION (FORWARD or REVERSE).
   */
  MOVE_DIRECTION getMoveDirection() { return m_current_move_direction; }

  /**
   * @brief Change the direction of movement to the opposite one.
   */
  void changeMoveDirection();

  /**
   * @brief Get the length of the train.
   * @return The total length of the train including margins.
   */
  float getLength() const;

  /**
   * @brief Get the current segment based on the movement direction.
   * @return The current SplineSegmentPtr.
   */
  Unigine::SplineSegmentPtr getCurrentSegment() const;

  /**
   * @brief Get the current parameterized position on the segment.
   * @return The current t-coordinate on the segment.
   */
  float getCurrentParamPos() const;

  /**
   * @brief Get the position information of the front bogie.
   * @return The BogiePos structure for the front bogie.
   */
  const BogiePos getFBogiePos() const { return m_forward_bogie_pos; }

  /**
   * @brief Get the position information of the back bogie.
   * @return The BogiePos structure for the back bogie.
   */
  const BogiePos getBBogiePos() const { return m_back_bogie_pos; }

 protected:
  /**
   * @brief Move the train along the path.
   */
  void moveTrain();

  std::shared_ptr<IMovementLogic> m_movement_logic = nullptr;

  /**
   * @brief Render visual debug information for the given node via Visualizer.
   * @param node Node to be rendered.
   */
  void renderNode(Unigine::NodePtr node);

 private:
  /**
   * @brief Calculate the position of the second bogie relative to the first
   * bogie. Here the ro
   * @param first First bogie position information.
   * @param second Second bogie position information.
   * @return Updated BogiePos structure for the second bogie.
   */
  BogiePos calcBackBogiePos(BogiePos first, BogiePos second);

  /**
   * @brief Set the current segment for forward movement.
   * @param curr_segment Segment to be set.
   * @param pos Position on the segment (default: 0).
   */
  void setSegmentForward(Unigine::SplineSegmentPtr curr_segment,
                         Unigine::SplineSegmentPtr prev_segment,
                         float pos = 0.f);

  /**
   * @brief Set the current segment for reverse movement.
   * @param curr_segment Segment to be set.
   * @param pos Position on the segment (default: 0).
   */
  void setSegmentReverse(Unigine::SplineSegmentPtr curr_segment,
                         Unigine::SplineSegmentPtr prev_segment,
                         float pos = 0.f);

  /**
   * @brief Internal initialization method.
   */
  void init();

  /**
   * @brief Internal update method called every frame.
   */
  void update();

 protected:
  /**
   * @brief Margin distance between train parts.
   */
  static constexpr float TRAINS_MARGIN = 1.f;

  /**
   * @brief Static counter for instances of this component.
   */
  inline static int m_count = 0;

  /**
   * @brief Current linear velocity of the train.
   */
  float m_current_linear_velocity = start_speed;

  /**
   * @brief New target linear velocity of the train.
   */
  float m_new_linear_velocity = start_speed;

  /**
   * @brief Acceleration rate of the train.
   */
  float m_acceleration = 1.f;

  /**
   * @brief Flag indicating whether the train is stopped.
   */
  bool m_is_stop = false;

  /**
   * @brief Distance between the two bogies.
   */
  float m_bogie_distance = 0.f;

  /**
   * @brief Current direction of movement.
   */
  MOVE_DIRECTION m_current_move_direction = MOVE_DIRECTION::FORWARD;

  /**
   * @brief Previous segment of the path.
   */
  Unigine::SplineSegmentPtr m_prev_segment;

  /**
   * @brief Forward bogie node.
   */
  Unigine::NodePtr m_forward_bogie_node;

  /**
   * @brief Back bogie node.
   */
  Unigine::NodePtr m_back_bogie_node;

  /**
   * @brief Car body node.
   */
  Unigine::NodePtr m_car_node;

  /**
   * @brief Position information for the front bogie.
   */
  BogiePos m_forward_bogie_pos;

  /**
   * @brief Position information for the back bogie.
   */
  BogiePos m_back_bogie_pos;

  /**
   * @brief Position information for the car body.
   */
  BogiePos m_car_pos;

  /**
   * @brief Callback function invoked to stop whole carriage.
   */
  std::function<void()> m_callback_move_start;

  /**
   * @brief Callback function invoked to start whole carriage movement.
   */
  std::function<void()> m_callback_move_stop;
};
