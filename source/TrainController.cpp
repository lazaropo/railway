#include "TrainController.h"

/**
 * @brief Регистрация компонента TrainController в системе компонентов Unigine.
 */
REGISTER_COMPONENT(TrainController);

using namespace Unigine;

/**
 * @brief Инициализирует компонент TrainController.
 *
 * Метод находит узлы для передней и задней тележек, а также кузова поезда.
 * Проверяется корректность начальных значений скорости и рассчитывается
 * расстояние между тележками.
 */
void TrainController::init() {
  NodePtr train_node = getNode();

  // Поиск узла передней тележки
  NodePtr node = train_node->findNode("BogieFront", true);
  m_forward_bogie_node = node;
  // if(node->getType() == Node::REFERENCE){
  //   Log::message("This is NodeReference.\n");

  //   NodeReferencePtr ref_node = checked_ptr_cast<NodeReference>(node);
  //   if(World::isUnpackNodeReferences())
  //     int a = 0;
  //   //const char* str = ref_node->getSrcFilePath();
  //   // Log::message("File NodeReference from: %s\n",
  //   ref_node->getSrcFilePath()); if(ref_node) {
  //     for(int i =0; i < node->getNumChildren(); ++i){
  //       ObjectMeshStaticPtr mesh =
  //       checked_ptr_cast<ObjectMeshStatic>(node->getChild(i)); if(mesh){
  //         Math::BoundBox bbox = mesh->getBoundBox();
  //         m_forward_bogie_node = node->getChild(i);
  //       }
  //     }

  // NodePtr loaded_node = ref_node->findNode("BogieFront");
  //       auto bbox = loaded_node->getBoundBox();
  //       if(bbox.isValid()){
  //         m_forward_bogie_node = loaded_node;
  //       } else
  //         Log::message("BBox is empty\n");

  //   } else
  //     Log::message("Ref Node is nullptr.\n");

  // }

  // Поиск узла задней тележки
  NodeReferencePtr ref =
      checked_ptr_cast<NodeReference>(getNode()->findNode("BogieBack", true));
  node = ref;
  // getNode()->findNode("BogieBack", true);
  m_back_bogie_node = node;
  // m_back_bogie_node->setSaveToWorldEnabled(true);

  // Поиск узла кузова поезда
  ref = checked_ptr_cast<NodeReference>(getNode()->findNode("Body", true));
  node = ref;
  m_car_node = node;
  //  m_car_node->setSaveToWorldEnabled(true);

  // Проверка корректности начальных скоростей
  if (start_speed > max_speed) {
    m_current_linear_velocity = max_speed;
    m_new_linear_velocity = max_speed;

    start_speed = max_speed;
  }

  // Расчет расстояния между тележками
  if (m_forward_bogie_node && m_back_bogie_node)
    m_bogie_distance =
        (m_forward_bogie_node->getPosition() - m_back_bogie_node->getPosition())
            .length();
}

/**
 * @brief Обновляет состояние компонента на каждом кадре.
 *
 * Если поезд остановлен или отсутствуют необходимые узлы, обновление
 * пропускается. В противном случае происходит расчет нового положения поезда и
 * его визуализация.
 */
void TrainController::update() {
  if (!m_forward_bogie_node || !m_back_bogie_node || !m_car_node) return;

  if (m_is_stop) return;

  // Пересчет расстояния между тележками, если оно слишком маленькое
  if (Math::abs(m_bogie_distance) < Math::Consts::EPS && m_forward_bogie_node &&
      m_back_bogie_node)
    m_bogie_distance =
        (m_forward_bogie_node->getPosition() - m_back_bogie_node->getPosition())
            .length();

  // Перемещение поезда
  moveTrain();

  // Проверка наличия сегментов пути для тележек
  if (!m_forward_bogie_pos.m_curr_segment || !m_back_bogie_pos.m_curr_segment)
    return;

  // Вычисление позиций тележек и кузова
  Math::Vec3 forward_pos = m_forward_bogie_pos.m_curr_segment->calcPoint(
      m_forward_bogie_pos.m_t_coordinate);
  Math::Vec3 back_pos = m_back_bogie_pos.m_curr_segment->calcPoint(
      m_back_bogie_pos.m_t_coordinate);
  Math::Vec3 car_pos = (forward_pos - back_pos) / 2 + back_pos;

  // Вычисление направления движения поезда
  Math::Vec3 car_direction = forward_pos - back_pos;

  // Установка глобальной ориентации и позиции поезда
  getNode()->setWorldDirection(Math::vec3(car_direction), Math::vec3_up,
                               Math::AXIS_Y);
  getNode()->setWorldPosition(car_pos);

  // Установка направлений для тележек - ориентация тележек по касательной
  // сплайна
  m_forward_bogie_node->setWorldDirection(
      m_forward_bogie_pos.m_curr_segment->calcTangent(
          m_forward_bogie_pos.m_t_coordinate),
      Math::vec3_up, Math::AXIS_Y);

  m_back_bogie_node->setWorldDirection(
      m_back_bogie_pos.m_curr_segment->calcTangent(
          m_back_bogie_pos.m_t_coordinate),
      Math::vec3_up, Math::AXIS_Y);

  // Визуализация узлов поезда
  renderNode(m_car_node);
  renderNode(m_forward_bogie_node);
}

/**
 * @brief Возвращает общую длину поезда.
 * @return Длина поезда, включая зазоры между тележками. Пока считаю длину
 * поезда как расстояние между тележками.
 */
float TrainController::getLength() const {
  if (m_car_node) {
    Math::Mat4 transform = m_car_node->getWorldTransform();

    auto bound_box = m_car_node->getBoundBox();
    // if(m_car_node->isWorld()){
    auto size = bound_box.getSize();
    if (size == Math::vec3_zero) Log::warning("BoundBox has zero size.\n");
    return size.y * 2.f + TRAINS_MARGIN;
    // }
    //   else
    //     Log::warning("BoundBox is not valid!");
  } else
    return 0.f;
}

/**
 * @brief Изменяет направление движения поезда на противоположное.
 */
void TrainController::changeMoveDirection() {
  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    m_current_move_direction = MOVE_DIRECTION::REVERSE;
  else
    m_current_move_direction = MOVE_DIRECTION::FORWARD;
}

/**
 * @brief Перемещает поезд вдоль заданного пути.
 *
 * Метод вызывает логику движения для первой тележки, после чего рассчитывает
 * положение второй тележки относительно первой. Если достигнут конец пути,
 * вызывается функция завершения движения.
 */
void TrainController::moveTrain() {
  if (!m_forward_bogie_node || !m_back_bogie_node || !m_car_node ||
      !m_movement_logic)
    return;

  m_current_linear_velocity = m_new_linear_velocity;

  if (m_forward_bogie_pos.m_curr_segment &&
      m_current_move_direction == MOVE_DIRECTION::FORWARD) {
    // Перемещение передней тележки вперед
    BogiePos pos = m_movement_logic->moveFirstBogie(m_forward_bogie_pos,
                                                    m_current_linear_velocity);
    if (!pos.m_curr_segment)
      m_callback_move_stop();  // Остановка движения состава, если достигнут
                               // конец пути
    else
      m_forward_bogie_pos = pos;

    // Расчет положения задней тележки относительно передней
    m_back_bogie_pos = m_movement_logic->calcSecondBogiePos(
        m_forward_bogie_pos, m_back_bogie_pos, m_bogie_distance);

  } else {
    // Перемещение задней тележки вперед (обратное движение)
    BogiePos pos = m_movement_logic->moveFirstBogie(m_back_bogie_pos,
                                                    -m_current_linear_velocity);
    if (!pos.m_curr_segment)
      m_callback_move_stop();  // Остановка движения состава, если достигнут
                               // конец пути
    else
      m_back_bogie_pos = pos;

    // Расчет положения передней тележки относительно задней
    m_forward_bogie_pos = m_movement_logic->calcSecondBogiePos(
        m_back_bogie_pos, m_forward_bogie_pos, -m_bogie_distance);
  }
}

/**
 * @brief Отображает оси локального пространства узла с помощью Visualizer.
 * @param node Узел, для которого отображаются оси.
 */
void TrainController::renderNode(NodePtr node) {
  Math::Vec3 pos = node->getWorldPosition();
  Visualizer::renderVector(
      pos,
      pos + (Math::Vec3)m_car_node->getWorldDirection(Math::AXIS_X),  // Ось X
      Math::vec4_red);  // Цвет оси — красный
  Visualizer::renderVector(
      pos,
      pos + (Math::Vec3)m_car_node->getWorldDirection(Math::AXIS_Y),  // Ось Y
      Math::vec4_green);  // Цвет оси — зеленый
  Visualizer::renderVector(
      pos,
      pos + (Math::Vec3)m_car_node->getWorldDirection(Math::AXIS_Z),  // Ось Z
      Math::vec4_blue);  // Цвет оси — синий
}

/**
 * @brief Возвращает текущий сегмент пути, зависящий от направления движения.
 * @return Указатель на текущий сегмент пути.
 */
SplineSegmentPtr TrainController::getCurrentSegment() const {
  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    return m_forward_bogie_pos.m_curr_segment;
  else
    return m_back_bogie_pos.m_curr_segment;
}

/**
 * @brief Возвращает текущую параметрическую позицию на сегменте пути.
 * @return Текущая координата t на сегменте пути.
 */
float TrainController::getCurrentParamPos() const {
  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    return m_forward_bogie_pos.m_t_coordinate;
  else
    return m_back_bogie_pos.m_t_coordinate;
}

/**
 * @brief Устанавливает начальные сегменты пути и позицию для поезда.
 *
 * Устанавливает начальную позицию поезда на определенном сегменте пути.
 * Используется для размещения поезда на маршруте в начале симуляции.
 * @param curr_segment Текущий сегмент пути.
 * @param prev_segment Предыдущий сегмент пути.
 * @param pos Начальная позиция на сегменте (по умолчанию 0).
 */
void TrainController::setTrainAtSegment(SplineSegmentPtr curr_segment,
                                        SplineSegmentPtr prev_segment,
                                        float pos) {
  if (!curr_segment) return;

  if (m_current_move_direction == MOVE_DIRECTION::FORWARD)
    setSegmentForward(curr_segment, prev_segment, pos);
  else
    setSegmentReverse(curr_segment, prev_segment, pos);
}

/**
 * @brief Устанавливает начальный сегмент пути для движения вперед.
 *
 * Устанавливает начальное положение передней тележки на указанном сегменте
 * пути. Затем рассчитывается положение задней тележки относительно передней.
 * @param curr_segment Текущий сегмент пути.
 * @param prev_segment Предыдущий сегмент пути.
 * @param pos Начальная позиция на сегменте (по умолчанию 0).
 */
void TrainController::setSegmentForward(SplineSegmentPtr curr_segment,
                                        SplineSegmentPtr prev_segment,
                                        float pos) {
  BogiePos& first_bogie = m_forward_bogie_pos;
  BogiePos& second_bogie = m_back_bogie_pos;

  first_bogie.m_curr_segment = curr_segment;

  if (Math::abs(m_bogie_distance) < Math::Consts::EPS) return;

  Unigine::SplineSegmentPtr tmp_segment;

  float len = curr_segment->getLength();

  first_bogie.m_t_coordinate = Math::clamp(pos, 0.f, 1.f);
  first_bogie.m_curr_segment_len = len;

  if (first_bogie.m_t_coordinate * len < m_bogie_distance) {
    // Задняя тележка находится на предыдущем сегменте
    tmp_segment = prev_segment;

    second_bogie.m_curr_segment = tmp_segment;
    second_bogie.m_curr_segment_len = tmp_segment->getLength();
    float back_t_pos =
        1.f - (m_bogie_distance - first_bogie.m_t_coordinate * len) /
                  second_bogie.m_curr_segment_len;
    second_bogie.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);

  } else {
    // Обе тележки находятся на одном сегменте
    second_bogie.m_curr_segment = curr_segment;
    second_bogie.m_curr_segment_len = len;
    float back_t_pos = first_bogie.m_t_coordinate - m_bogie_distance / len;
    second_bogie.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);
  }
}

/**
 * @brief Устанавливает начальный сегмент пути для движения назад.
 *
 * Устанавливает начальное положение задней тележки на указанном сегменте пути.
 * Затем рассчитывается положение передней тележки относительно задней.
 * @param curr_segment Текущий сегмент пути.
 * @param prev_segment Предыдущий сегмент пути.
 * @param pos Начальная позиция на сегменте (по умолчанию 0).
 */
void TrainController::setSegmentReverse(SplineSegmentPtr curr_segment,
                                        SplineSegmentPtr prev_segment,
                                        float pos) {
  BogiePos& first_bogie = m_back_bogie_pos;
  BogiePos& second_bogie = m_forward_bogie_pos;

  first_bogie.m_curr_segment = curr_segment;

  if (Math::abs(m_bogie_distance) < Math::Consts::EPS) return;

  Unigine::SplineSegmentPtr tmp_segment;

  float len = curr_segment->getLength();

  first_bogie.m_t_coordinate = Math::clamp(pos, 0.f, 1.f);
  first_bogie.m_curr_segment_len = len;

  if ((1.f - first_bogie.m_t_coordinate) * len < m_bogie_distance) {
    // Передняя тележка находится на предыдущем сегменте
    tmp_segment = prev_segment;

    second_bogie.m_curr_segment = tmp_segment;
    second_bogie.m_curr_segment_len = tmp_segment->getLength();
    float back_t_pos =
        (m_bogie_distance - (1.f - first_bogie.m_t_coordinate) * len) /
        second_bogie.m_curr_segment_len;
    second_bogie.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);

  } else {
    // Обе тележки находятся на одном сегменте
    second_bogie.m_curr_segment = curr_segment;
    second_bogie.m_curr_segment_len = len;
    float back_t_pos = first_bogie.m_t_coordinate + m_bogie_distance / len;
    second_bogie.m_t_coordinate = Math::clamp(back_t_pos, 0.f, 1.f);
  }
}