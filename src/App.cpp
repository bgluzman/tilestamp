#include "App.h"

#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <misc/cpp/imgui_stdlib.h>

#include <array>
#include <iostream>
#include <stdexcept>

namespace ts {

App::App(SDL_Renderer *renderer) : renderer_(renderer) {}

App::~App() noexcept {
  if (image_) {
    SDL_DestroyTexture(image_);
  }
}

bool App::operator()(ImGuiIO & /*io*/, SDL_Window &window) {
  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
  // tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
  // your main application, or clear/overwrite your copy of the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
  // data to your main application, or clear/overwrite your copy of the
  // keyboard data. Generally you may always pass all inputs to dear imgui,
  // and hide them from your application based on those two flags.
  bool done = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_QUIT)
      done = true;
    if (event.type == SDL_WINDOWEVENT &&
        event.window.event == SDL_WINDOWEVENT_CLOSE &&
        event.window.windowID == SDL_GetWindowID(&window))
      done = true;

    if (event.type == SDL_DROPFILE) {
      char *path = event.drop.file;
      LoadImage(path);
      SDL_free(path);
    }
  }

  // We demonstrate using the full viewport area or the work area (without
  // menu-bars, task-bars etc.) Based on your use case you may want one or the
  // other.
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

  // Lay out UI using approach from here:
  // https://github.com/ocornut/imgui/issues/125#issuecomment-135775009
  static float w = 400.0f;
  static float h = 500.0f;
  static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                  ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoSavedSettings;

  ImGui::Begin("Hello, world!", nullptr, flags);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

  ImGui::BeginChild("child1", ImVec2(w, h), true);
  Properties();
  ImGui::SameLine();

  ImGui::EndChild();

  ImGui::SameLine();
  ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, h));
  if (ImGui::IsItemActive())
    w += ImGui::GetIO().MouseDelta.x;
  ImGui::SameLine();

  ImGui::BeginChild("child2", ImVec2(0, h), true);
  Tilemap();
  ImGui::EndChild();

  ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
  if (ImGui::IsItemActive())
    h += ImGui::GetIO().MouseDelta.y;

  ImGui::BeginChild("child3", ImVec2(0, 0), true);
  Output();
  ImGui::EndChild();

  ImGui::PopStyleVar();
  ImGui::End();

  return done;
}

void App::Properties() {
  if (ImGui::BeginTable("properties", 2)) {
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Value");
    ImGui::TableHeadersRow();

    for (const auto &[name, value] : properties_) {
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::SetNextItemWidth(-FLT_MIN);
      ImGui::Text(name.c_str());
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(-FLT_MIN);
      ImGui::Text(value.c_str());
    }

    ImGui::EndTable();
  }
}

void App::Tilemap() {
  if (image_) {
    static float scale = 1.0;
    ImGui::DragFloat("zoom", &scale, 0.01f);
    ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Top-left
    ImVec2 uv_max = ImVec2(1.0f, 1.0f); // Lower-right
    ImGui::Image(image_,
                 ImVec2(static_cast<float>(image_w_) * scale,
                        static_cast<float>(image_h_) * scale),
                 uv_min, uv_max, ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
                 ImGui::GetStyleColorVec4(ImGuiCol_Border));
  } else {
    ImGui::Text("no image selected");
  }
}

void App::Output() {
  ImGui::TextWrapped(
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed volutpat "
      "eros diam, auctor tristique dui tempor et. Suspendisse potenti. Nam "
      "porta at erat et consectetur. Cras in molestie enim. In sodales euismod "
      "leo quis malesuada. Phasellus gravida eget nunc a imperdiet. Curabitur "
      "convallis elementum ex, quis scelerisque neque tincidunt sit amet. Cras "
      "ultricies libero sed tempor pulvinar. Proin at mauris dignissim velit "
      "vestibulum efficitur at at enim. Duis ac dui eget lorem molestie "
      "vehicula id hendrerit nibh. Suspendisse elementum sapien sed leo "
      "vulputate semper. Quisque interdum luctus mauris vel tempus. Sed "
      "imperdiet eros sit amet sem congue, non molestie neque euismod.");
}

void App::LoadImage(const std::filesystem::path &path) {
#ifdef __WIN32
  std::array<char, 1024> char_buf{'\0'};
  std::size_t path_bytes =
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

  if (SDL_QueryTexture(base_image, nullptr, nullptr, &image_w_, &image_h_) <
      0) {
    throw std::runtime_error{"SDL_QueryTexture"};
  }

  image_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ABGR8888,
                             SDL_TEXTUREACCESS_TARGET, image_w_, image_h_);
  if (!image_) {
    throw std::runtime_error{"SDL_CreateTexture"};
  }

  SDL_Texture *prev_render_target = SDL_GetRenderTarget(renderer_);

  if (SDL_SetRenderTarget(renderer_, image_) < 0) {
    std::cerr << SDL_GetError() << std::endl;
    throw std::runtime_error{"SDL_SetRenderTarget"};
  }
  SDL_Rect rect{.x = 0, .y = 0, .w = image_w_, .h = image_h_};
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

} // namespace ts