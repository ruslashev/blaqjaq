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

enum class game_state_t {
    no_game
  , ask_starting_money
  , ask_bet
  , temp_next
};
static game_state_t game_state = game_state_t::no_game;
static int money = 1000, bet = 0;

static void load() {
  glClearColor(187 / 255.f, 206 / 255.f, 242 / 255.f, 1.f);
  c = new card_drawer(20.f, 15, 0.62f, 3.3f);
}

static void gui_display_current_state() {
  ImGui::Text("Game state:");
  ImGui::SameLine();
  switch (game_state) {
    case game_state_t::no_game:
      ImGui::Text("no game");
      break;
    case game_state_t::ask_starting_money:
      ImGui::Text("ask starting money");
      break;
    case game_state_t::ask_bet:
      ImGui::Text("ask bet");
      break;
    default:
      ImGui::Text("unknown");
      break;
  }
}

static void new_game() {
  money = bet = 0;
  game_state = game_state_t::ask_starting_money;
}

static void draw_gui() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::SmallButton("New game"))
      new_game();

    ImGui::SameLine();
    if (ImGui::SmallButton("Show test window"))
      show_test_window = !show_test_window;

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

  gui_display_current_state();

  switch (game_state) {
    case game_state_t::no_game:
      ImGui::Text("No match being played right now");
      break;
    case game_state_t::ask_starting_money:
      ImGui::Text("Money:");
      ImGui::SameLine();
      ImGui::PushItemWidth(200);
      ImGui::InputInt("", &money);
      ImGui::PopItemWidth();
      ImGui::SameLine();
      if (ImGui::Button("OK", ImVec2(100, 0)))
        if (money > 0)
          game_state = game_state_t::ask_bet;
      break;
    case game_state_t::ask_bet:
      ImGui::Text("Money: %d", money);
      ImGui::Text("Enter bet:");
      ImGui::SameLine();
      ImGui::PushItemWidth(200);
      ImGui::InputInt("", &bet);
      ImGui::PopItemWidth();
      ImGui::SameLine();
      if (ImGui::Button("OK", ImVec2(100, 0)))
        if (bet > 0 && bet <= money)
          game_state = game_state_t::temp_next;
      break;
    case game_state_t::temp_next:
      ImGui::Text("Money: %d", money);
      ImGui::Text("Bet: %d", bet);
    default:
      break;
  }

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

static void key_event(char key, bool down) {
}
static void mouse_motion_event(float xrel, float yrel, int x, int y) {
}
static void mouse_button_event(int button, bool down, int x, int y) {
}
static void update(double dt, double t) {
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

