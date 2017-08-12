#include "gfx/driver.hh"
#include <imgui/imgui.h>
#include "gfx/ogl.hh"
#include "gfx/utils.hh"
#include <memory>
#include <GL/glew.h>

static bool show_test_window = false;

static void load() {
  glClearColor(0.07f, 0.07f, 0.07f, 0.07f);
}

static void key_event(char key, bool down) {
}

static void mouse_motion_event(float xrel, float yrel, int x, int y) {
}

static void mouse_button_event(int button, bool down, int x, int y) {
}

static void update(double dt, double t) {
}

static void frame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (ImGui::BeginMainMenuBar()) {
    {
      if (ImGui::SmallButton("Show test window"))
        show_test_window = !show_test_window;
    }
    ImGui::EndMainMenuBar();
  }

  if (show_test_window) {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow(&show_test_window);
  }

  // float screen_aspect_ratio = (float)ImGui::GetIO().DisplaySize.x
  //   / (float)ImGui::GetIO().DisplaySize.y;
  // glm::mat4 projection = glm::perspective((float)glm::radians(fov)
  //     , screen_aspect_ratio, 0.1f, 300.f), view = cam->compute_view_mat()
  //   , model = glm::rotate(glm::rotate(glm::mat4(), glm::radians(dragging_y)
  //         , glm::vec3(1, 0, 0)), glm::radians(dragging_x), glm::vec3(0, 1, 0))
  //   , mvp = projection * view * model;
}

static void cleanup() {
}

int main() {
  driver_init("poqer", 976, 732);

  load();

  driver_main_loop(frame, update, key_event, mouse_motion_event
      , mouse_button_event);

  cleanup();

  driver_destroy();

  return 0;
}

