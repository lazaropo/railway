#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

#include <memory>

#include "Bogie.h"

/**
 * @class Carriage
 * @brief Компонент, представляющий вагон в игровом движке Unigine.
 *
 * Этот класс определяет компонент, который управляет набором контроллеров
 * поездов (представляющих отдельные вагоны поезда). Он предоставляет методы для
 * управления движением всех подключенных контроллеров поездов одновременно.
 */
class Carriage : public Unigine::ComponentBase {
 public:
  /**
   * @brief Определяем тип компонента как Carriage.
   */
  COMPONENT_DEFINE(Carriage, Unigine::ComponentBase);

  // PROP_PARAM(Node, carriage_node);

  /**
   * @brief Инициализация компонента.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Обновление компонента на каждом кадре.
   */
  COMPONENT_UPDATE(update);

  void setPosition(SegmentPosition pos);

  void setStartPosition(SegmentPosition pos);

  float getLength() const {
    if (m_body)
      return m_body->getBoundBox().getSize().y;
    else
      return 0.f;
  }

 protected:
  /**
   * @brief Инициализация компонента.
   */
  void init();

  /**
   * @brief Обновление компонента на каждом кадре.
   */
  void update();

  void moveBackBogie();

  Unigine::NodePtr m_body;

  std::unique_ptr<Bogie*> m_forward_bogie = nullptr;
  std::unique_ptr<Bogie*> m_back_bogie = nullptr;

  // Bogie* m_forward_bogie;
  // Bogie* m_back_bogie = nullptr;

  float m_distance_btw_bogie;

  SegmentPosition m_position;
};