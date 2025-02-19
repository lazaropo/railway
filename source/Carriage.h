#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

#include "TrainController.h"

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

  /**
   * @brief Инициализация компонента.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Обновление компонента на каждом кадре.
   */
  COMPONENT_UPDATE(update);

  /**
   * @brief Останавливает движение всех поездов, связанных с данным составом.
   */
  void stopMove() {
    for (auto train : m_trains) train->stopMove();
  }

  /**
   * @brief Запускает движение всех поездов, связанных с данным составом.
   */
  void startMove() {
    for (auto train : m_trains) train->startMove();
  }

  /**
   * @brief Возвращает значение обратной частоты кадров (IFPS). Хранимое
   * значение IFps обновляется каждые n-вызовов, где n - количество вагонов в
   * составе.
   *
   * @return Значение IFPS, полученное с помощью Unigine::Game::getIFps().
   */
  float getIfps() {
    if (m_count++ == m_trains.size()) {
      m_count = 0;
      m_ifps = Unigine::Game::getIFps();
    }
    return m_ifps;
  }

 protected:
  /**
   * @brief Инициализация компонента.
   */
  void init() {}

  /**
   * @brief Обновление компонента на каждом кадре.
   */
  void update() {}

  /**
   * @brief Устанавливает поезда, связанные с данным составом.
   */
  void setTrains();

  /**
   * @brief Виртуальный метод для установки стартовой позиции вагона. Сделан
   * виртуальным, потому что здесь предполагается использование методов из
   * @class TrainManager, но @class Carriage это база для прочих реализаций
   * контроллеров состава.
   *
   * Производные классы могут переопределять этот метод для реализации
   * собственного поведения.
   */
  virtual void setStartPos() {}

  /**
   * @brief Контейнер, содержащий указатели на поезда, управляемые данным
   * составом.
   *
   * Порядок отражает расположение вагонов состава: индекс 0 соответствует
   * голове поезда, а индекс N - 1 — хвосту.
   */
  Unigine::Vector<TrainController*> m_trains;

  /**
   * @brief Обратная частота кадров (IFPS), используемая для расчетов.
   */
  float m_ifps = 0.f;

  /**
   * @brief Счетчик, используемый для отслеживания количество вызовов функции
   * @fn getIfps().
   */
  size_t m_count = m_trains.size();
};