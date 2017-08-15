#include "gfx/driver.hh"
#include "gfx/driver.hh"
#include "gfx/card_drawer.hh"
#include "gfx/ogl.hh"
#include "card.hh"
#include "utils.hh"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui/imgui.h"
#include <memory>

static bool show_test_window = false;
static glm::mat4 projection_mat;
static card_drawer *c;

static deck_t deck;
static bool match_in_progress = false;
static int money = 1000, bet = 0;

static void load() {
  glClearColor(187 / 255.f, 206 / 255.f, 242 / 255.f, 1.f);

  deck = generate_deck(1);

  c = new card_drawer(20.f, 15, 0.62f, 3.3f);
}

static void key_event(char key, bool down) {
}
static void mouse_motion_event(float xrel, float yrel, int x, int y) {
}
static void mouse_button_event(int button, bool down, int x, int y) {
}
static void update(double dt, double t) {
}

static void new_game() {
  money = bet = 0;
  ImGui::OpenPopup("Enter starting balance");
  match_in_progress = true;
}

static void gui_ask_starting_balance_popup() {
  if (ImGui::BeginPopupModal("Enter starting balance", NULL
        , ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::InputInt("", &money);
    if (ImGui::Button("OK", ImVec2(100, 0)))
      ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }
}

static void draw_gui() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::SmallButton("New game"))
      new_game();

    gui_ask_starting_balance_popup();

    ImGui::SameLine();
    if (ImGui::SmallButton("Show test window"))
      show_test_window = !show_test_window;

    ImGui::SameLine();
    if (match_in_progress)
      ImGui::Text("money: %d", money);

    ImGui::EndMainMenuBar();
  }
  if (show_test_window) {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow(&show_test_window);
  }

  float padding = 5;
  ImGui::SetNextWindowPos(ImVec2(padding, 25 + padding), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x - padding * 2
        , ImGui::GetIO().DisplaySize.y - 25 - padding * 2), ImGuiCond_Always);
  ImGui::Begin("Main game", nullptr
      , ImGuiWindowFlags_NoTitleBar
      | ImGuiWindowFlags_NoResize
      | ImGuiWindowFlags_NoMove
      | ImGuiWindowFlags_NoCollapse);
  if (!match_in_progress)
    ImGui::Text("No match being played right now");

  ImGui::End();
}

static void frame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_gui();

  projection_mat = glm::ortho(0.f, (float)ImGui::GetIO().DisplaySize.x
      , (float)ImGui::GetIO().DisplaySize.y, 0.f, -1.f, 1.f);
}

static void cleanup() {
  delete c;
}

int main() {
  driver_init("blaqjaq", 976, 732);

  load();

  driver_main_loop(frame, update, key_event, mouse_motion_event
      , mouse_button_event);

  cleanup();

  driver_destroy();

  return 0;
}

