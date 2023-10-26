#ifndef TILESTAMP_APP_H
#define TILESTAMP_APP_H

#include <SDL.h>
#include <imgui.h>

#include <filesystem>
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
  void Properties();
  void Tilemap();
  void Output();

  void LoadImage(const std::filesystem::path &path);

  SDL_Renderer *renderer_;
  SDL_Texture *image_ = nullptr;

  std::vector<std::pair<std::string, std::string>> properties_ = {
      {"default", "default"}};

  int image_w_ = 0;
  int image_h_ = 0;
};

} // namespace ts

#endif // TILESTAMP_APP_H
