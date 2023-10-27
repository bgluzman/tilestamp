#ifndef TILESTAMP_TILESHEETDISPLAY_H
#define TILESTAMP_TILESHEETDISPLAY_H

#include <SDL.h>

#include <filesystem>

namespace ts {

class TilesheetDisplay {
 public:
  explicit TilesheetDisplay(SDL_Renderer *renderer);
  ~TilesheetDisplay() noexcept;
  TilesheetDisplay(TilesheetDisplay&&) noexcept;
  TilesheetDisplay& operator=(TilesheetDisplay&&) noexcept;
  TilesheetDisplay(const TilesheetDisplay&) = delete;
  TilesheetDisplay& operator=(const TilesheetDisplay&) = delete;

  void LoadImage(const std::filesystem::path& path);
  void Draw(float scale);

 private:
  SDL_Renderer *renderer_;
  SDL_Texture  *image_ = nullptr;

  int display_width_ = 0;
  int display_height_ = 0;
};

}  // namespace ts

#endif  // TILESTAMP_TILESHEETDISPLAY_H
