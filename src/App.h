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

  void LoadOutput(const std::filesystem::path &path);
  void LoadImage(const std::filesystem::path &path);

  SDL_Renderer *renderer_;
  SDL_Texture *image_ = nullptr;

  std::string metadata_ = "Drag-and-drop JSON file here to load.";

  std::vector<std::pair<std::string, std::string>> properties_ = {
      {"field1", "value1"},
      {"field2", "value2"},
  };

  int image_w_ = 0;
  int image_h_ = 0;
};

} // namespace ts

#endif // TILESTAMP_APP_H
