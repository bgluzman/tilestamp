#ifndef TILESTAMP_APP_H
#define TILESTAMP_APP_H

#include <SDL.h>
#include <imgui.h>

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "TilesheetDisplay.h"

namespace ts {

class App {
 public:
  explicit App(SDL_Renderer *renderer);
  ~App() noexcept = default;
  App(const App&) = delete;
  App& operator=(const App&) = delete;
  App(App&&) noexcept = default;
  App& operator=(App&&) noexcept = default;

  bool operator()(ImGuiIO& io, SDL_Window& window);

 private:
  void Properties();
  void Output();

  void LoadOutput(const std::filesystem::path& path);

  TilesheetDisplay tilesheet_display_;

  std::string metadata_ = "Drag-and-drop JSON file here to load.";

  std::vector<std::pair<std::string, std::string>> properties_ = {
      {"field1", "value1"},
      {"field2", "value2"},
  };
};

}  // namespace ts

#endif  // TILESTAMP_APP_H
