#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineInput.h>

class InputController : public Unigine::ComponentBase {
 private:
  static InputController* m_instance;
  static bool m_is_enabled;
  static bool m_is_action;

 public:
  COMPONENT_DEFINE(InputController, Unigine::ComponentBase);

  COMPONENT_INIT(init);
  COMPONENT_UPDATE(update);

  enum INPUT_ACTIONS {
    SPEED_DECREASE,
    SPEED_INCREASE,
    TURN_AROUND,
    WORLD_CAMERA_SWITCH,
    LOCAL_CAMERA_SWITCH
  };

  inline static bool isEnabled() { return m_is_enabled; }
  inline static InputController* getInstance() { return m_instance; }
  inline static bool isAction() { return m_is_action; }

  static float getActionState(INPUT_ACTIONS action);

  class InputStateKeyDown {
   private:
    Unigine::Input::KEY m_key = Unigine::Input::KEY::KEY_UNKNOWN;

   public:
    InputStateKeyDown(Unigine::Input::KEY key) : m_key(key) {}

    float getState() { return Unigine::Input::isKeyDown(m_key) ? 1.f : 0.f; }
  };

 private:
  class InputAction {
   private:
    Unigine::Vector<InputStateKeyDown*> m_states;

   public:
    float m_state = 0;

    InputAction(Unigine::Vector<InputStateKeyDown*> states)
        : m_states(states) {}

    float update() {
      float new_state = -1;

      for (InputStateKeyDown* it : m_states)
        new_state = std::max(new_state, it->getState());

      m_state = new_state;

      return m_state;
    }
  };

  InputAction m_actions[5] = {
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_UP)})),
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_DOWN)})),
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_R)})),
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_F2)})),
      InputAction(Unigine::Vector<InputStateKeyDown*>(
          {new InputStateKeyDown(Unigine::Input::KEY_F5)}))};

 protected:
  void init();
  void update();
};
