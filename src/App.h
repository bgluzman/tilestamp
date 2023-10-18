#ifndef TILESTAMP_APP_H
#define TILESTAMP_APP_H

#include <SDL.h>
#include <imgui.h>

#include <string>
#include <utility>
#include <vector>

namespace ts {

class App {
public:
  // TODO (bgluzman): copy/move ctor/assignment
  App(SDL_Renderer *renderer);
  ~App() noexcept;

  bool operator()(ImGuiIO &io, SDL_Window &window);

private:
  void MenuBar();
  void Properties();
  void Tilemap();
  void Output();

  std::vector<std::pair<std::string, std::string>> properties_ = {
      {"default", "default"}};
  SDL_Texture *image_ = nullptr;
};

} // namespace ts

#endif // TILESTAMP_APP_H
