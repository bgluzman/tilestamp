#include "TilesheetDisplay.h"

#include <SDL_image.h>
#include <imgui.h>

#include <array>
#include <iostream>
#include <utility>

#include "imgui.h"

namespace ts {

TilesheetDisplay::TilesheetDisplay(SDL_Renderer *renderer)
    : renderer_(renderer) {}

TilesheetDisplay::~TilesheetDisplay() noexcept {
  if (image_) {
    SDL_DestroyTexture(image_);
  }
}

TilesheetDisplay::TilesheetDisplay(TilesheetDisplay&& other) noexcept
    : renderer_(std::exchange(other.renderer_, nullptr)),
      image_(std::exchange(other.image_, nullptr)),
      display_width_(std::exchange(other.display_width_, 0)),
      display_height_(std::exchange(other.display_height_, 0)) {}

TilesheetDisplay& TilesheetDisplay::operator=(TilesheetDisplay&& rhs) noexcept {
  if (this != &rhs) {
    renderer_ = std::exchange(rhs.renderer_, nullptr);
    image_ = std::exchange(rhs.image_, nullptr);
    display_width_ = std::exchange(display_width_, 0);
    display_height_ = std::exchange(display_height_, 0);
  }
  return *this;
}

void TilesheetDisplay::LoadImage(const std::filesystem::path& path) {
#ifdef __WIN32
  std::array<char, 1024> char_buf{'\0'};
  std::size_t            path_bytes =
      std::wcstombs(char_buf.data(), path.c_str(), sizeof(char_buf));
  if (path_bytes == static_cast<std::size_t>(-1)) {
    throw std::runtime_error{"std::wcstombs"};
  }
  char_buf[std::min(path_bytes, sizeof(char_buf) - 1)] = '\0';
  SDL_Texture *base_image = IMG_LoadTexture(renderer_, char_buf.data());
#else
  SDL_Texture *base_image = IMG_LoadTexture(renderer_, path.c_str());
#endif
  if (!base_image) {
    // TODO (bgluzman): obviously change this later...
    // throw std::runtime_error{"cannot load image"};
    return;
  }

  if (SDL_QueryTexture(base_image, nullptr, nullptr, &display_width_,
                       &display_height_) < 0) {
    throw std::runtime_error{"SDL_QueryTexture"};
  }

  image_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ABGR8888,
                             SDL_TEXTUREACCESS_TARGET, display_width_,
                             display_height_);
  if (!image_) {
    throw std::runtime_error{"SDL_CreateTexture"};
  }

  SDL_Texture *prev_render_target = SDL_GetRenderTarget(renderer_);

  if (SDL_SetRenderTarget(renderer_, image_) < 0) {
    std::cerr << SDL_GetError() << std::endl;
    throw std::runtime_error{"SDL_SetRenderTarget"};
  }
  SDL_Rect rect{.x = 0, .y = 0, .w = display_width_, .h = display_height_};
  if (SDL_RenderCopy(renderer_, base_image, &rect, &rect) < 0) {
    throw std::runtime_error{"SDL_RenderCopy"};
  }

  if (SDL_SetRenderDrawColor(renderer_, 0, 255, 0, SDL_ALPHA_OPAQUE) < 0) {
    throw std::runtime_error{"SDL_SetRenderDrawColor"};
  }
  SDL_Rect selection{.x = 0, .y = 0, .w = 16, .h = 16};
  if (SDL_RenderDrawRect(renderer_, &selection) < 0) {
    throw std::runtime_error{"SDL_RenderDrawRect"};
  }

  if (SDL_SetRenderTarget(renderer_, prev_render_target) < 0) {
    std::cerr << SDL_GetError() << std::endl;
    throw std::runtime_error{"SDL_SetRenderTarget"};
  }
}

void TilesheetDisplay::Draw(float scale) {
  static constexpr ImVec2 kUV0{0.0f, 0.0f};
  static constexpr ImVec2 kUV1{1.0f, 1.0f};
  static constexpr ImVec4 kTint{1.0f, 1.0f, 1.0f, 1.0f};
  if (image_) {
    ImVec2 size{static_cast<float>(display_width_) * scale,
                static_cast<float>(display_height_) * scale};
    ImGui::Image(image_, size, kUV0, kUV1, kTint,
                 ImGui::GetStyleColorVec4(ImGuiCol_Border));
  } else {
    ImGui::Text("no image selected");
  }
}

}  // namespace ts
