#pragma once

#include <UnigineComponentSystem.h>

#include <memory>

#include "Carriage.h"
#include "InputController.h"
#include "MovementLogic.h"
#include "TrainManager.h"

/**
 * @class CarriagePlayer
 * @brief Класс расширяющий базовый класс Carriage для контроля состава игроком.
 *
 * Это класс-контроллер добавляет функциональность для управления составом и
 * камерами в игре.
 */
class CarriagePlayer : public Carriage {
 public:
  /**
   * @brief Определяем тип компонента как CarriagePlayer.
   */
  COMPONENT_DEFINE(CarriagePlayer, Carriage);

  /**
   * @brief Параметр для камеры на голове поезда.
   */
  PROP_PARAM(Node, head_camera);

  /**
   * @brief Параметр для камеры на хвосте поезда.
   */
  PROP_PARAM(Node, tail_camera);

  /**
   * @brief Параметр для общей камеры мира.
   */
  PROP_PARAM(Node, world_camera);

  /**
   * @brief Инициализация компонента.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Обновление компонента на каждом кадре.
   */
  COMPONENT_UPDATE(update);

 protected:
  /**
   * @brief Устанавливает начальную позицию для поездов из состава.
   *
   * Переопределяемый метод для настройки стартовой позиции состава.
   */
  void setStartPos() override;

  /**
   * @brief Метод инициализации.
   */
  void init();

  /**
   * @brief Метод обновления, вызываемый на каждом кадре.
   */
  void update();

 private:
  /**
   * @brief Указатель на камеру на голове поезда.
   */
  Unigine::PlayerSpectatorPtr m_head_camera;

  /**
   * @brief Указатель на камеру на хвосте поезда.
   */
  Unigine::PlayerSpectatorPtr m_tail_camera;

  /**
   * @brief Указатель на общую камеру мира.
   */
  Unigine::PlayerSpectatorPtr m_world_camera;
};