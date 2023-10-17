#ifndef TILESTAMP_APP_H
#define TILESTAMP_APP_H

#include <SDL.h>
#include <imgui.h>

namespace ts {

class App {
public:
  bool operator()(ImGuiIO &io, SDL_Window &window);

private:
  void MenuBar();
  void AttributesFrame(ImGuiIO &io);
  void TilemapFrame();
  void OutputFrame();

  int counter_ = 0;
};

} // namespace ts

#endif // TILESTAMP_APP_H
