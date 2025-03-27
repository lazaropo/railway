#pragma once

#include <UnigineComponentSystem.h>

#include "InputController.h"
#include "Train.h"

class MainPlayer : public Unigine::ComponentBase {
 public:
  COMPONENT_DEFINE(MainPlayer, Unigine::ComponentBase);

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

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

 protected:
  void init();
  void update();

  Train* m_train;

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
