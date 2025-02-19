#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineInput.h>

/**
 * @class InputController
 * @brief Компонент для обработки входных данных от пользователя.
 *
 * Этот компонент регистрирует и обрабатывает нажатия клавиш, обеспечивая
 * управление различными действиями в игре, такими как изменение скорости, смена
 * направления движения и переключение камер.
 */
class InputController : public Unigine::ComponentBase {
 private:
  /**
   * @brief Указатель на единственный экземпляр компонента InputController.
   */
  static InputController* m_instance;

  /**
   * @brief Флаг, показывающий, включен ли компонент.
   */
  static bool m_is_enabled;

  /**
   * @brief Флаг, показывающий, активировано ли какое-либо действие.
   */
  static bool m_is_action;

 public:
  /**
   * @brief Определение типа компонента как InputController.
   */
  COMPONENT_DEFINE(InputController, Unigine::ComponentBase);

  /**
   * @brief Инициализация компонента.
   */
  COMPONENT_INIT(init);

  /**
   * @brief Обновление состояния компонента на каждом кадре.
   */
  COMPONENT_UPDATE(update);

  /**
   * @enum INPUT_ACTIONS
   * @brief Перечисление возможных действий, управляемых через клавиатуру.
   */
  enum INPUT_ACTIONS {
    SPEED_DECREASE,  ///< Уменьшение скорости
    SPEED_INCREASE,  ///< Увеличение скорости
    TURN_AROUND,  ///< Изменение направления движения
    WORLD_CAMERA_SWITCH,  ///< Переключение на глобальную камеру
    LOCAL_CAMERA_SWITCH  ///< Переключение на локальную камеру
  };

  /**
   * @brief Проверяет, включен ли компонент.
   * @return True, если компонент включен, иначе False.
   */
  inline static bool isEnabled() { return m_is_enabled; }

  /**
   * @brief Возвращает указатель на единственный экземпляр компонента.
   * @return Указатель на экземпляр InputController.
   */
  inline static InputController* getInstance() { return m_instance; }

  /**
   * @brief Проверяет, активировано ли какое-либо действие.
   * @return True, если любое действие активно, иначе False.
   */
  inline static bool isAction() { return m_is_action; }

  /**
   * @brief Возвращает состояние определенного действия.
   *
   * Этот метод возвращает текущее состояние одного из действий, определенных в
   * перечислении INPUT_ACTIONS. Состояние может принимать значения от 0 до 1,
   * где 1 означает активное состояние.
   *
   * @param action Действие, состояние которого нужно получить.
   * @return Состояние действия.
   */
  static float getActionState(INPUT_ACTIONS action);

  /**
   * @class InputStateKeyDown
   * @brief Класс для обработки состояний нажатий клавиш.
   *
   * Этот класс хранит информацию о состоянии определенной клавиши и
   * предоставляет метод для проверки, была ли эта клавиша нажата.
   */
  class InputStateKeyDown {
   private:
    /**
     * @brief Код клавиши, которую нужно отслеживать.
     */
    Unigine::Input::KEY m_key = Unigine::Input::KEY::KEY_UNKNOWN;

   public:
    /**
     * @brief Конструктор класса InputStateKeyDown.
     *
     * @param key Код клавиши, которую нужно отслеживать.
     */
    InputStateKeyDown(Unigine::Input::KEY key) : m_key(key) {}

    /**
     * @brief Возвращает состояние клавиши.
     *
     * Метод возвращает 1, если клавиша нажата, и 0, если клавиша не нажата.
     *
     * @return Состояние клавиши.
     */
    float getState() { return Unigine::Input::isKeyDown(m_key) ? 1.f : 0.f; }
  };

 private:
  /**
   * @class InputAction
   * @brief Класс для обработки действий, основанных на состояниях клавиш.
   *
   * Этот класс объединяет несколько состояний клавиш и позволяет отслеживать
   * общее состояние действия, основанное на состояниях отдельных клавиш.
   */
  class InputAction {
   private:
    /**
     * @brief Вектор объектов InputStateKeyDown, связанных с данным действием.
     */
    Unigine::Vector<InputStateKeyDown*> m_states;

   public:
    /**
     * @brief Общее состояние действия.
     */
    float m_state = 0;

    /**
     * @brief Конструктор класса InputAction.
     *
     * @param states Вектор объектов InputStateKeyDown, связанных с данным
     * действием.
     */
    InputAction(Unigine::Vector<InputStateKeyDown*> states)
        : m_states(states) {}

    /**
     * @brief Обновляет состояние действия.
     *
     * Метод обновляет общее состояние действия, основываясь на состояниях
     * отдельных клавиш. Если хотя бы одна клавиша активна, состояние действия
     * устанавливается равным 1.
     *
     * @return Новое состояние действия.
     */
    float update() {
      float new_state = -1;

      // Проходим по всем состояниям клавиш и выбираем максимальное значение
      for (InputStateKeyDown* it : m_states)
        new_state = std::max(new_state, it->getState());

      // Обновляем общее состояние действия
      m_state = new_state;

      return m_state;
    }
  };

  /**
   * @brief Массив объектов InputAction для обработки различных действий.
   */
  InputAction m_actions[5] = {
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_DOWN)})),
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_UP)})),
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_R)})),
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_F2)})),
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_F3)}))};

 protected:
  /**
   * @brief Инициализирует компонент InputController.
   */
  void init();

  /**
   * @brief Обновляет состояние компонента на каждом кадре.
   */
  void update();
};