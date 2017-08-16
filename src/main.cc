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
  , dealing_cards
};
static game_state_t game_state = game_state_t::no_game;
static int money = 1000, bet = 0;
static deck_t player_hand, dealer_hand;
static std::vector<int> player_scores, dealer_scores;

static void load() {
  glClearColor(39 / 255.f, 119 / 255.f, 20 / 255.f, 1.f); // casino green
  // glClearColor(187 / 255.f, 206 / 255.f, 242 / 255.f, 1.f);
  c = new card_drawer(11.f, 15, 0.62f, 3.3f);
  srand(time(nullptr));
}

static void gui_display_current_state() {
  ImGuiStyle& style = ImGui::GetStyle();
  // set bg alpha to 0 because I can't draw cards on top of ui
  style.Colors[ImGuiCol_WindowBg].w = 0;
  // style.Colors[ImGuiCol_Text] = ImVec4(0.07f, 0.07f, 0.07f, 1.f);

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
    case game_state_t::dealing_cards:
      ImGui::Text("dealing cards");
      break;
    default:
      ImGui::Text("unknown");
      break;
  }
}

static void gui_draw_scores(const std::vector<int> &scores) {
  for (size_t i = 0; i < scores.size(); ++i)
    if (i != scores.size() - 1) {
      ImGui::Text("%d or", scores[i]);
      ImGui::SameLine();
    } else
      ImGui::Text("%d", scores[i]);
}

static void draw_hand(const deck_t &deck, float x, float y, float xoff) {
  float x_draw = x;
  for (const card_t &card : deck) {
    c->draw(card, x_draw, y, projection_mat);
    x_draw += xoff;
  }
}

static void game_state_new_game() {
  money = bet = 1000; // TODO temporary for quicker testing
  game_state = game_state_t::ask_starting_money;
}

static void game_state_deal_cards() {
  game_state = game_state_t::dealing_cards;
  player_hand = deal_hand();
  dealer_hand = deal_hand();
  player_scores = count_scores(player_hand);
  dealer_scores = count_scores(dealer_hand);
}

static void draw_gui() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::SmallButton("New game"))
      game_state_new_game();

    ImGui::SameLine();
    if (ImGui::SmallButton("Show test window"))
      show_test_window = !show_test_window;

    ImGui::SameLine();
    gui_display_current_state();

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
      | ImGuiWindowFlags_ShowBorders
      | ImGuiWindowFlags_NoCollapse);

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
      ImGui::Text("Bet:");
      ImGui::SameLine();
      ImGui::PushItemWidth(200);
      ImGui::InputInt("", &bet);
      ImGui::PopItemWidth();
      ImGui::SameLine();
      if (ImGui::Button("OK", ImVec2(100, 0)))
        if (bet > 0 && bet <= money)
          game_state_deal_cards();
      break;
    case game_state_t::dealing_cards:
      ImGui::Text("Money: %d", money);
      ImGui::Text("Bet: %d", bet);
      ImGui::Text("Dealer's hand:");
      ImGui::SameLine();
      gui_draw_scores(dealer_scores);
      draw_hand(dealer_hand, 14, 109, 60);
      ImGui::Text("\n\n\n\n"); // I know
      ImGui::Text("Your hand:");
      ImGui::SameLine();
      gui_draw_scores(player_scores);
      draw_hand(player_hand, 14, 109 + 100, 60);
    default:
      break;
  }

  ImGui::End();
}

static void frame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  projection_mat = glm::ortho(0.f, (float)ImGui::GetIO().DisplaySize.x
      , (float)ImGui::GetIO().DisplaySize.y, 0.f, -1.f, 1.f);

  draw_gui();
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
  driver_init("blaqjaq", 700, 525);

  load();

  driver_main_loop(frame, update, key_event, mouse_motion_event
      , mouse_button_event);

  cleanup();

  driver_destroy();

  return 0;
}

