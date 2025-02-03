#ifndef __APP_WORLD_LOGIC_H__
#define __APP_WORLD_LOGIC_H__

#include <UnigineFileSystem.h>
#include <UnigineLogic.h>
#include <UnigineStreams.h>
#include <UnigineWorlds.h>

// #include "Train.h"

class AppWorldLogic : public Unigine::WorldLogic {
 public:
  AppWorldLogic();
  ~AppWorldLogic() override;

  int init() override;

  int update() override;
  int postUpdate() override;
  int updatePhysics() override;

  int shutdown() override;

  int save(const Unigine::StreamPtr &stream) override;
  int restore(const Unigine::StreamPtr &stream) override;

 private:
  // Инициализируем сплайн по filepath
  // Unigine::String m_spline_filepath = "spline.spl";
  // Unigine::WorldSplineGraphPtr m_graph;

  // // Контейнер для поездов
  // Unigine::Vector<Train*> m_trains;
};

#endif  // __APP_WORLD_LOGIC_H__
