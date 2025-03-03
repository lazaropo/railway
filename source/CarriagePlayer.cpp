#include "CarriagePlayer.h"

/**
 * @brief Регистрирует компонент CarriagePlayer в системе компонентов Unigine.
 */
REGISTER_COMPONENT(CarriagePlayer);

using namespace Unigine;

/**
 * @brief Инициализация компонента CarriagePlayer.
 *
 * Этот метод настраивает логику движения для каждого поезда,
 * связанного с компонентом, и устанавливает основную камеру для игрока.
 */
void CarriagePlayer::init() {
  Vector<NodePtr> nodes;
  World::getNodes(nodes);
  for (int i = 0; i < nodes.size(); ++i) {
    NodeReferencePtr ptr = checked_ptr_cast<NodeReference>(nodes[i]);
    if (ptr) {
      Log::message("NodeReference is found: %s (ID: %d)\n",
                   ptr->getInfo().get(), ptr->getID());
      ptr->setEnabled(false);
      ptr->setEnabled(true);
      if (!ptr->isEnabled()) {
        ptr->setEnabled(true);
        Log::message("NodeReference is activated: %s\n", ptr->getInfo().get());
      }
    }
  }

  // Устанавливаем указатель на текущий экземпляр состава в TrainManager
  TrainManager::setCarriage(this);

  // Устанавливаем поезда, связанные с данным составом
  setTrains();

  // Если есть хотя бы один поезд, создаем объект с логикой движения
  if (m_trains.size()) {
    // Создаем объект логики движения
    std::shared_ptr<IMovementLogic> logic(new MovementLogic());

    // Устанавливаем функцию обратного вызова для получения предыдущих и
    // следующих сегментов пути
    logic->setPrevSegmentFunction(&TrainManager::getPrevSegment);
    logic->setNextSegmentFunction(&TrainManager::getNextSegment);

    // Устанавливаем функцию для получения IFPS
    logic->setGetIfpsFunction(std::bind(&Carriage::getIfps, this));

    // Назначаем созданную логику каждому контроллеру поезда
    for (TrainController* train : m_trains) train->setMovementLogic(logic);

    // Логируем сообщение о количестве загруженных поездов
    Log::message("Carriage Player: %d trains are loaded\n", m_trains.size());
  }

  // Получаем указатели на камеры
  m_head_camera = checked_ptr_cast<PlayerSpectator>(head_camera.get());
  m_tail_camera = checked_ptr_cast<PlayerSpectator>(tail_camera.get());
  m_world_camera = checked_ptr_cast<PlayerSpectator>(world_camera.get());

  // Устанавливаем камеру на голове состава основной
  if (m_head_camera) m_head_camera->setMainPlayer(true);
}

/**
 * @brief Обновление компонента на каждом кадре.
 *
 * Этот метод обрабатывает ввод пользователя, такие как ускорение, замедление и
 * переключение камер.
 */
void CarriagePlayer::update() {
  // Если первый сегмент пути отсутствует, устанавливаем начальную позицию
  if (!m_trains[0]->getCurrentSegment()) setStartPos();

  // Переменная для временного хранения значений действий ввода
  float tmp = 0;

  // Проверяем, нажата ли кнопка уменьшения скорости
  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_DECREASE);
  if (Math::abs(tmp) > Math::Consts::EPS)
    for (auto it : m_trains) it->brake();  // Замедляем все поезда

  // Проверяем, нажата ли кнопка увеличения скорости
  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_INCREASE);
  if (Math::abs(tmp) > Math::Consts::EPS)
    for (auto it : m_trains) it->accelerate();  // Ускоряем все поезда

  // Флаг для переключения камеры
  bool is_local_camera_switch = false;

  // Проверяем, нажата ли кнопка смены направления движения
  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::TURN_AROUND)) {
    for (auto train : m_trains) {
      train->changeMoveDirection();  // Меняем направление движения
      train->startMove();  // Начинаем движение
    }
    is_local_camera_switch = true;  // Включаем переключение камеры
  }

  // Проверяем, нажата ли кнопка переключения на мировую камеру
  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::WORLD_CAMERA_SWITCH)) {
    m_world_camera->setMainPlayer(true);  // Делаем мировую камеру основной
    m_head_camera->setMainPlayer(false);  // Отключаем камеру головы
    m_tail_camera->setMainPlayer(false);  // Отключаем камеру хвоста
  }

  // Проверяем, нажата ли кнопка переключения на локальную камеру
  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::LOCAL_CAMERA_SWITCH))
    is_local_camera_switch = true;  // Включаем переключение камеры

  // Переключаемся на камеру головы, если движется вперед
  if (is_local_camera_switch && m_trains[0]->getMoveDirection() ==
                                    TrainController::MOVE_DIRECTION::FORWARD) {
    m_world_camera->setMainPlayer(false);  // Отключаем мировую камеру
    m_head_camera->setMainPlayer(true);  // Включаем камеру головы
    m_tail_camera->setMainPlayer(false);  // Отключаем камеру хвоста
  }

  // Переключаемся на камеру хвоста, если движется назад
  if (is_local_camera_switch && m_trains[0]->getMoveDirection() ==
                                    TrainController::MOVE_DIRECTION::REVERSE) {
    m_world_camera->setMainPlayer(false);  // Отключаем мировую камеру
    m_head_camera->setMainPlayer(false);  // Отключаем камеру головы
    m_tail_camera->setMainPlayer(true);  // Включаем камеру хвоста
  }

  // Устанавливаем позиции камер над головным и хвостовым вагонами
  m_head_camera->setWorldPosition(
      (*m_trains.front())->getNode()->getPosition() + Math::Vec3_up * 5);
  m_tail_camera->setWorldPosition((*m_trains.back())->getNode()->getPosition() +
                                  Math::Vec3_up * 5);
}

/**
 * @brief Устанавливает начальную позицию для каждого поезда в составе.
 *
 * Этот метод последовательно расставляет поезда вдоль заданного пути, начиная с
 * первого сегмента и двигаясь в обратном направлении, учитывая длины вагонов.
 */
void CarriagePlayer::setStartPos() {
  if (m_trains.empty()) return;  // Если нет поездов, выходим

  // Получаем начальный и предыдущий сегменты пути
  SplineSegmentPtr current_segment = TrainManager::getStartSegment();
  SplineSegmentPtr prev_segment = TrainManager::getPrevSegment(current_segment);

  // Длина текущего сегмента пути
  int len = current_segment->getLength();
  float pos = 0.f;  // Позиция на сегменте

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
    auto train_len = (*it)->getLength();
    pos -= (train_len + 2.f) / len;
    if (pos < 0.f) {
      // Переходим на предыдущий сегмент
      current_segment = prev_segment;
      prev_segment = TrainManager::getPrevSegment(current_segment);

      // Пересчитываем позицию на предыдущем сегменте
      pos = 1.f - (-pos) * len / current_segment->getLength();
      len = current_segment->getLength();
    }
  }
}
