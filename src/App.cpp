#include "App.h"

#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <misc/cpp/imgui_stdlib.h>

#include <iostream>
#include <stdexcept>

namespace ts {

App::App(SDL_Renderer *renderer) {
  // TODO (bgluzman): better logging/error reporting everywhere
  SDL_Texture *base_image = IMG_LoadTexture(renderer, "");
  if (!base_image) {
    throw std::runtime_error{"cannot load image"};
  }

  if (SDL_QueryTexture(base_image, nullptr, nullptr, &image_w_, &image_h_) <
      0) {
    throw std::runtime_error{"SDL_QueryTexture"};
  }

  image_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                             SDL_TEXTUREACCESS_TARGET, image_w_, image_h_);
  if (!image_) {
    throw std::runtime_error{"SDL_CreateTexture"};
  }

  SDL_Texture *prev_render_target = SDL_GetRenderTarget(renderer);

  if (SDL_SetRenderTarget(renderer, image_) < 0) {
    std::cerr << SDL_GetError() << std::endl;
    throw std::runtime_error{"SDL_SetRenderTarget"};
  }
  SDL_Rect rect{.x = 0, .y = 0, .w = image_w_, .h = image_h_};
  if (SDL_RenderCopy(renderer, base_image, &rect, &rect) < 0) {
    throw std::runtime_error{"SDL_RenderCopy"};
  }

  if (SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE) < 0) {
    throw std::runtime_error{"SDL_SetRenderDrawColor"};
  }
  SDL_Rect selection{.x = 0, .y = 0, .w = 16, .h = 16};
  if (SDL_RenderDrawRect(renderer, &selection) < 0) {
    throw std::runtime_error{"SDL_RenderDrawRect"};
  }

  if (SDL_SetRenderTarget(renderer, prev_render_target) < 0) {
    std::cerr << SDL_GetError() << std::endl;
    throw std::runtime_error{"SDL_SetRenderTarget"};
  }
}

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
  }

  MenuBar();
  Tilemap();
  Properties();
  Output();

  return done;
}

void App::MenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ImGui::MenuItem("(demo menu)", NULL, false, false);
      if (ImGui::MenuItem("Checked", NULL, true)) {
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Quit", "Alt+F4")) {
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Undo", "CTRL+Z")) {
      }
      if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
      } // Disabled item
      ImGui::Separator();
      if (ImGui::MenuItem("Cut", "CTRL+X")) {
      }
      if (ImGui::MenuItem("Copy", "CTRL+C")) {
      }
      if (ImGui::MenuItem("Paste", "CTRL+V")) {
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void App::Properties() {
  ImGui::Begin("Properties");

  if (ImGui::BeginTable("properties", 3)) {
    ImGui::TableSetupColumn("Name");
    ImGui::TableSetupColumn("Value");
    ImGui::TableSetupColumn("");
    ImGui::TableHeadersRow();

    bool delete_row = false;
    int row = 0;
    for (; row != properties_.size(); ++row) {
      auto &[name, value] = properties_[row];
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::InputText(("##name" + std::to_string(row)).c_str(), &name);
      ImGui::TableSetColumnIndex(1);
      ImGui::InputText(("##value" + std::to_string(row)).c_str(), &value);
      ImGui::TableSetColumnIndex(2);
      if (ImGui::Button(("-##" + std::to_string(row)).c_str())) {
        delete_row = true;
        break;
      }
    }

    if (delete_row)
      properties_.erase(properties_.begin() + row);

    ImGui::EndTable();
  }

  if (ImGui::Button("+")) {
    properties_.push_back({"name", "value"});
  }
  ImGui::End();
}

void App::Tilemap() {
  ImGui::Begin("Tilemap", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

  static float scale = 1.0;
  ImGui::DragFloat("zoom", &scale, 0.01f);

  ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Top-left
  ImVec2 uv_max = ImVec2(1.0f, 1.0f); // Lower-right
  ImGui::Image(image_, ImVec2(image_w_ * scale, image_h_ * scale), uv_min,
               uv_max, ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
               ImGui::GetStyleColorVec4(ImGuiCol_Border));

  ImGui::End();
}

void App::Output() {
  ImGui::Begin("Output");
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
  ImGui::End();
}

} // namespace ts