#include "MainPlayer.h"

/**
 * @brief Регистрирует компонент MainPlayer в системе компонентов Unigine.
 */
REGISTER_COMPONENT(MainPlayer);

using namespace Unigine;

/**
 * @brief Инициализация компонента MainPlayer.
 *
 * Этот метод настраивает логику движения для каждого поезда,
 * связанного с компонентом, и устанавливает основную камеру для игрока.
 */
void MainPlayer::init() {
  //   Vector<NodePtr> nodes;
  //   World::getNodes(nodes);
  //   for (int i = 0; i < nodes.size(); ++i) {
  //     NodeReferencePtr ptr = checked_ptr_cast<NodeReference>(nodes[i]);
  //     if (ptr) {
  //       Log::message("NodeReference is found: %s (ID: %d)\n",
  //                    ptr->getInfo().get(), ptr->getID());
  //       ptr->setEnabled(false);
  //       ptr->setEnabled(true);
  //       if (!ptr->isEnabled()) {
  //         ptr->setEnabled(true);
  //         Log::message("NodeReference is activated: %s\n",
  //         ptr->getInfo().get());
  //       }
  //     }
  //   }

  //   // Устанавливаем указатель на текущий экземпляр состава в TrainManager
  //   TrainManager::setCarriage(this);

  m_train = std::make_shared<Train*>(
      ComponentSystem::get()->getComponent<Train>(getNode()));

  //   // Устанавливаем поезда, связанные с данным составом
  //   setTrains();

  //   // Если есть хотя бы один поезд, создаем объект с логикой движения
  //   if (m_trains.size()) {
  //     // Создаем объект логики движения
  //     std::shared_ptr<IMovementLogic> logic(new MovementLogic());

  //     // Устанавливаем функцию обратного вызова для получения предыдущих и
  //     // следующих сегментов пути
  //     logic->setPrevSegmentFunction(&TrainManager::getPrevSegment);
  //     logic->setNextSegmentFunction(&TrainManager::getNextSegment);

  //     // Устанавливаем функцию для получения IFPS
  //     logic->setGetIfpsFunction(std::bind(&Carriage::getIfps, this));

  //     // Назначаем созданную логику каждому контроллеру поезда
  //     for (TrainController* train : m_trains) train->setMovementLogic(logic);

  //     // Логируем сообщение о количестве загруженных поездов
  //     Log::message("Carriage Player: %d trains are loaded\n",
  //     m_trains.size());
  //   }

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
void MainPlayer::update() {
  // Если первый сегмент пути отсутствует, устанавливаем начальную позицию
  if (!m_train) return;

  // Переменная для временного хранения значений действий ввода
  float tmp = 0;

  // Проверяем, нажата ли кнопка уменьшения скорости
  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_DECREASE);
  if (Math::abs(tmp) > Math::Consts::EPS)
    (*m_train)->brake();  // Замедляем все поезда

  // Проверяем, нажата ли кнопка увеличения скорости
  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_INCREASE);
  if (Math::abs(tmp) > Math::Consts::EPS)
    (*m_train)->accelerate();  // Ускоряем все поезда

  // Флаг для переключения камеры
  bool is_local_camera_switch = false;

  // Проверяем, нажата ли кнопка смены направления движения
  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::TURN_AROUND)) {
    (*m_train)->changeMoveDirection();  // Меняем направление движения
    // (*m_train)->startMove();  // Начинаем движение

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
  if (is_local_camera_switch &&
      (*m_train)->getMoveDirection() == Carriage::MOVE_DIRECTION::FORWARD) {
    m_world_camera->setMainPlayer(false);  // Отключаем мировую камеру
    m_head_camera->setMainPlayer(true);  // Включаем камеру головы
    m_tail_camera->setMainPlayer(false);  // Отключаем камеру хвоста
  }

  // Переключаемся на камеру хвоста, если движется назад
  if (is_local_camera_switch &&
      (*m_train)->getMoveDirection() == Carriage::MOVE_DIRECTION::REVERSE) {
    m_world_camera->setMainPlayer(false);  // Отключаем мировую камеру
    m_head_camera->setMainPlayer(false);  // Отключаем камеру головы
    m_tail_camera->setMainPlayer(true);  // Включаем камеру хвоста
  }

  // Устанавливаем позиции камер над головным и хвостовым вагонами
  m_head_camera->setWorldPosition((*m_train)->getFrontWorldPosition() +
                                  Math::Vec3_up * 5);
  m_tail_camera->setWorldPosition((*m_train)->getBackWorldPosition() +
                                  Math::Vec3_up * 5);
}
