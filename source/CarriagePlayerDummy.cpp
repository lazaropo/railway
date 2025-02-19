#include "CarriagePlayerDummy.h"

/**
 * @brief Регистрирует компонент CarriagePlayerDummy в системе компонентов
 * Unigine.
 */
REGISTER_COMPONENT(CarriagePlayerDummy);

using namespace Unigine;

/**
 * @brief Инициализация компонента CarriagePlayerDummy.
 *
 * Этот метод настраивает начальную скорость движения для каждого поезда,
 * связанного с компонентом, и устанавливает логику движения.
 */
void CarriagePlayerDummy::init() {
  // Устанавливаем указатель на текущий экземпляр состава в менеджере поездов
  TrainManager::setCarriage(this);

  // Устанавливаем поезда, связанные с данным составом
  setTrains();

  // Если указана постоянная скорость движения, применяем её для всех поездов
  if (const_move_speed > Math::Consts::EPS)
    for (TrainController* train : m_trains)
      train->start_speed = const_move_speed;

  // Если есть хотя бы один поезд, создаем логику движения
  if (m_trains.size()) {
    // Создаем объект логики движения
    std::shared_ptr<IMovementLogic> logic(new MovementLogic());

    // Устанавливаем функции обратного вызова для получения предыдущих и
    // следующих сегментов пути
    logic->setPrevSegmentFunction(&TrainManager::getPrevSegment);
    logic->setNextSegmentFunction(&TrainManager::getNextSegment);

    // Устанавливаем функцию для получения IFPS
    logic->setGetIfpsFunction(std::bind(&Carriage::getIfps, this));

    // Назначаем созданную логику каждому контроллеру поезда
    for (TrainController* train : m_trains) train->setMovementLogic(logic);
  }
}

/**
 * @brief Обновление компонента на каждом кадре.
 *
 * Этот метод проверяет наличие текущего сегмента пути и, если его нет,
 * устанавливает начальную позицию.
 */
void CarriagePlayerDummy::update() {
  // Если текущий сегмент пути отсутствует, устанавливаем начальную позицию
  if (!m_trains[0]->getCurrentSegment()) setStartPos();
}

/**
 * @brief Устанавливает начальную позицию для каждого поезда в составе.
 *
 * Этот метод последовательно расставляет поезда вдоль заданного пути, начиная с
 * первого сегмента и двигаясь в обратном направлении, учитывая длины вагонов.
 */
void CarriagePlayerDummy::setStartPos() {
  if (m_trains.empty()) return;  // Если нет поездов, выходим

  // Получаем начальный и предыдущий сегменты пути
  SplineSegmentPtr current_segment = TrainManager::getStartSegment();
  SplineSegmentPtr prev_segment = TrainManager::getPrevSegment(current_segment);

  // Длина текущего сегмента пути
  int len = current_segment->getLength();
  float pos = 0.f;  // Начальная позиция на сегменте

  // Проходим по всем поездам в составе
  for (auto it = m_trains.begin(), it_end = m_trains.end(); it != it_end;
       ++it) {
    if (!current_segment)
      break;  // Если сегмент пути закончился, прерываем цикл

    // Устанавливаем поезд на текущем сегменте пути
    (*it)->setTrainAtSegment(current_segment, prev_segment, pos);

    // Если длина сегмента слишком мала, продолжаем на следующем сегменте
    if (Math::abs(len) < Math::Consts::EPS) continue;

    // Рассчитываем новую позицию на сегменте
    pos -= ((*it)->getLength() + 2.f) / len;
    if (pos < 0.f) {
      // Переходим на предыдущий сегмент
      current_segment = prev_segment;
      prev_segment = TrainManager::getPrevSegment(current_segment);

      // Пересчитываем позицию на новом сегменте
      pos = 1.f - (-pos) * len / current_segment->getLength();
      len = current_segment->getLength();
    }
  }
}