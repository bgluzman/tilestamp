#include "App.h"

#include <SDL.h>
#include <SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>

#include <array>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <stdexcept>

namespace ts {

using namespace nlohmann;

App::App(SDL_Renderer *renderer) : tilesheet_display_(renderer) {}

bool App::operator()(ImGuiIO& /*io*/, SDL_Window& window) {
  // Poll and handle events (inputs, window resize, etc.)
  // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
  // tell if dear imgui wants to use your inputs.
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
  // your main application, or clear/overwrite your copy of the mouse data.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
  // data to your main application, or clear/overwrite your copy of the
  // keyboard data. Generally you may always pass all inputs to dear imgui,
  // and hide them from your application based on those two flags.
  bool      done = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (event.type == SDL_QUIT) done = true;
    if (event.type == SDL_WINDOWEVENT &&
        event.window.event == SDL_WINDOWEVENT_CLOSE &&
        event.window.windowID == SDL_GetWindowID(&window))
      done = true;

    if (event.type == SDL_DROPFILE) {
      std::string path{event.drop.file};
      if (path.ends_with(".json")) {
        LoadOutput(path);
      } else {
        tilesheet_display_.LoadImage(path);
      }
      SDL_free(event.drop.file);
    }
  }

  // We demonstrate using the full viewport area or the work area (without
  // menu-bars, task-bars etc.) Based on your use case you may want one or the
  // other.
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

  ImVec2       winsize = ImGui::GetWindowSize();
  static float w_scale = winsize.x / 1280.0f;
  static float h_scale = winsize.y / 720.0f;

  // Lay out UI using approach from here:
  // https://github.com/ocornut/imgui/issues/125#issuecomment-135775009
  static float            w = 400.0f * w_scale;
  static float            h = 500.0f * h_scale;
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
  if (ImGui::IsItemActive()) w += ImGui::GetIO().MouseDelta.x;
  ImGui::SameLine();

  ImGui::BeginChild("child2", ImVec2(0, h), true);
  tilesheet_display_.Draw(1.0f);  // TODO (bgluzman): scaling
  ImGui::EndChild();

  ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
  if (ImGui::IsItemActive()) h += ImGui::GetIO().MouseDelta.y;

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

    for (const auto& [name, value] : properties_) {
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

void App::Output() { ImGui::TextWrapped(metadata_.c_str()); }

void App::LoadOutput(const std::filesystem::path& path) {
  std::ifstream     in(path);
  std::stringstream buffer;
  buffer << in.rdbuf();
  metadata_ = buffer.str();
}

}  // namespace ts