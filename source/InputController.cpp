#include "InputController.h"

/**
 * @brief Регистрация компонента InputController в системе компонентов Unigine.
 */
REGISTER_COMPONENT(InputController);

using namespace Unigine;

/**
 * @brief Указатель на единственный экземпляр компонента InputController.
 */
InputController* InputController::m_instance = nullptr;

/**
 * @brief Флаг, определяющий активность компонента.
 */
bool InputController::m_is_enabled = false;

/**
 * @brief Флаг, определяющий активность какого-либо действия.
 */
bool InputController::m_is_action = false;

/**
 * @brief Инициализация компонента InputController.
 *
 * Этот метод устанавливает указатель на текущий экземпляр компонента и включает
 * его.
 */
void InputController::init() {
  m_instance = this;
  m_is_enabled = true;
  m_is_action = false;
}

/**
 * @brief Обновление состояния компонента на каждом кадре.
 *
 * Этот метод обновляет состояние всех действий, проверяя, было ли выполнено
 * какое-либо действие. Если хотя бы одно действие активно, соответствующий флаг
 * устанавливается в true.
 */
void InputController::update() {
  m_is_action = false;

  // Обновляем состояние каждого действия
  for (InputAction it : m_actions)
    if (it.update()) m_is_action = true;
}

/**
 * @brief Возвращает состояние определенного действия.
 *
 * Этот метод обновляет состояние указанного действия и возвращает его текущее
 * значение. Если компонент отключен, возвращается 0.
 *
 * @param action Действие, состояние которого нужно получить.
 * @return Состояние действия.
 */
float InputController::getActionState(INPUT_ACTIONS action) {
  if (!m_instance || !m_is_enabled) return 0.f;

  // Обновляем состояние выбранного действия
  m_instance->m_actions[(int)action].update();

  // Возвращаем текущее состояние действия
  return m_instance->m_actions[(int)action].m_state;
}
