#include "App.h"

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>

namespace ts {

bool App::operator()(ImGuiIO &io, SDL_Window &window) {
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

  // We demonstrate using the full viewport area or the work area (without
  // menu-bars, task-bars etc.) Based on your use case you may want one or the
  // other.
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

  static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                  ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_NoSavedSettings;
  ImGui::Begin("Hello, world!", nullptr, flags);

  ImGui::Text("Text goes here.");

  if (ImGui::Button("Button"))
    counter_++;
  ImGui::SameLine();
  ImGui::Text("counter = %d", counter_);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / io.Framerate, io.Framerate);
  ImGui::End();

  return done;
}

} // namespace ts