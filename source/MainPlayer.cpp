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
  m_train = ComponentSystem::get()->getComponent<Train>(getNode());

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
    m_train->brake();  // Замедляем пользовательский поезд

  // Проверяем, нажата ли кнопка увеличения скорости
  tmp = InputController::getInstance()->getActionState(
      InputController::INPUT_ACTIONS::SPEED_INCREASE);
  if (Math::abs(tmp) > Math::Consts::EPS)
    m_train->accelerate();  // Ускоряем пользовательский поезд

  // Флаг для переключения камеры
  bool is_local_camera_switch = false;

  // Проверяем, нажата ли кнопка смены направления движения
  if (InputController::getInstance()->getActionState(
          InputController::INPUT_ACTIONS::TURN_AROUND)) {
    m_train->changeMoveDirection();  // Меняем направление движения

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
      m_train->getMoveDirection() == Carriage::MOVE_DIRECTION::FORWARD) {
    m_world_camera->setMainPlayer(false);  // Отключаем мировую камеру
    m_head_camera->setMainPlayer(true);  // Включаем камеру головы
    m_tail_camera->setMainPlayer(false);  // Отключаем камеру хвоста
  }

  // Переключаемся на камеру хвоста, если движется назад
  if (is_local_camera_switch &&
      m_train->getMoveDirection() == Carriage::MOVE_DIRECTION::REVERSE) {
    m_world_camera->setMainPlayer(false);  // Отключаем мировую камеру
    m_head_camera->setMainPlayer(false);  // Отключаем камеру головы
    m_tail_camera->setMainPlayer(true);  // Включаем камеру хвоста
  }

  // Устанавливаем позиции камер над головным и хвостовым вагонами.
  // Math::Vec3_up * 5 - перемещаем камеру вверх от вагона.
  m_head_camera->setWorldPosition(m_train->getFrontWorldPosition() +
                                  Math::Vec3_up * 5);
  m_tail_camera->setWorldPosition(m_train->getBackWorldPosition() +
                                  Math::Vec3_up * 5);
}
