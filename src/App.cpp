#include "App.h"

#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <misc/cpp/imgui_stdlib.h>

#include <stdexcept>

namespace ts {

App::App(SDL_Renderer *renderer) {
  image_ = IMG_LoadTexture(renderer, "");
  if (!image_) {
    throw std::runtime_error{"cannot load image"};
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

    int row = 0;
    for (auto &[name, value] : properties_) {
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::InputText(("##name" + std::to_string(row)).c_str(), &name);
      ImGui::TableSetColumnIndex(1);
      ImGui::InputText(("##value" + std::to_string(row)).c_str(), &value);
      ImGui::TableSetColumnIndex(2);
      ImGui::Button(("-##" + std::to_string(row)).c_str());
      ++row;
    }

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
  int my_tex_w = 0, my_tex_h = 0;
  if (SDL_QueryTexture(image_, nullptr, nullptr, &my_tex_w, &my_tex_h) < 0) {
    // TODO (bgluzman): handle this in or somewhere better?
    throw std::runtime_error{"SDL_QueryTexture"};
  }
  ImGui::Image(image_, ImVec2(my_tex_w * scale, my_tex_h * scale), uv_min,
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