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
  , tie
  , player_natural
  , dealer_natural
  , the_play
  , player_bust
  , dealer_plays
  , dealer_bust
  , player_won
  , dealer_won
};
static game_state_t game_state = game_state_t::no_game;
static int money = 1000, bet = 0;
static deck_t player_hand, dealer_hand, dealer_shown_hand;
static std::vector<int> player_scores, dealer_scores, dealer_first_card_scores;
static bool draw_all_dealer_cards = false;

static void load() {
  srand(time(nullptr));
  glClearColor(39 / 255.f, 119 / 255.f, 20 / 255.f, 1.f); // casino green
  c = new card_drawer(11.f, 15, 0.62f, 3.3f);

  ImGuiStyle& style = ImGui::GetStyle();
  // set bg alpha to 0 because I can't figure out how to draw cards on top of ui
  style.Colors[ImGuiCol_WindowBg].w = 0;
}

/// score closest but not exceeding 21
static int best_score(const std::vector<int> &scores) {
  int prev_score = scores[0];
  for (int score : scores)
    if (score > 21)
      return prev_score;
    else
      prev_score = score;
  return prev_score;
}

static void game_state_new_game() {
  money = bet = 1000; // TODO temporary for quicker testing
  game_state = game_state_t::ask_starting_money;
}

static void game_state_deal_cards() {
  draw_all_dealer_cards = false;

  game_state = game_state_t::dealing_cards;

  player_hand = deal_hand();
  player_scores = count_scores(player_hand);

  dealer_hand = deal_hand();
  dealer_scores = count_scores(dealer_hand);
  deck_t dealer_hand_first_card = { dealer_hand[0] };
  dealer_first_card_scores = count_scores(dealer_hand_first_card);

  if (scores_have_bj(player_scores) && scores_have_bj(dealer_scores)) {
    money += bet;
    game_state = game_state_t::tie;
  } if (scores_have_bj(player_scores)) {
    money += std::round((double)bet * 2.5);
    game_state = game_state_t::player_natural;
  } else if (scores_have_bj(dealer_scores))
    game_state = game_state_t::dealer_natural;
  else
    game_state = game_state_t::the_play;
}

static void game_dealer_playing_logic() {
  // dealer hits on soft 17
  // note this accounts for hands with ace
  if (dealer_scores[0] < 17) {
    dealer_hand.push_back(random_card());
    dealer_scores = count_scores(dealer_hand);
    game_dealer_playing_logic();
  }
}

static void game_dealer_plays() {
  game_state = game_state_t::dealer_plays;
  game_dealer_playing_logic();
  if (dealer_scores[0] > 21) {
    money += bet * 2;
    game_state = game_state_t::dealer_bust;
  } else if (best_score(dealer_scores) == best_score(player_scores)) {
    money += bet;
    game_state = game_state_t::tie;
  } else if (best_score(dealer_scores) > best_score(player_scores))
    game_state = game_state_t::dealer_won;
  else if (best_score(dealer_scores) < best_score(player_scores)) {
    money += bet * 2;
    game_state = game_state_t::player_won;
  }
}

static void game_hit() {
  player_hand.push_back(random_card());
  player_scores = count_scores(player_hand);
  if (scores_have_bj(player_scores))
    game_dealer_plays();
  if (player_scores[0] > 21)
    game_state = game_state_t::player_bust;
}

static void game_continue() {
  if (money == 0) {
    ImGui::Text("\n");
    ImGui::Text("u lost it all even in virtual blackjack.");
    ImGui::Text("protip: seek help on ur addiction");
  } else
    if (ImGui::Button("OK"))
      game_state = game_state_t::ask_bet;
}

static void gui_display_current_state() {
  std::string state;
  switch (game_state) {
    case game_state_t::no_game:            state = "no game"; break;
    case game_state_t::ask_starting_money: state = "ask starting money"; break;
    case game_state_t::ask_bet:            state = "ask bet"; break;
    case game_state_t::dealing_cards:      state = "dealing cards"; break;
    case game_state_t::tie:                state = "tie"; break;
    case game_state_t::player_natural:     state = "player natural"; break;
    case game_state_t::dealer_natural:     state = "dealer natural"; break;
    case game_state_t::the_play:           state = "the play"; break;
    case game_state_t::player_bust:        state = "player bust"; break;
    case game_state_t::dealer_plays:       state = "dealer plays"; break;
    case game_state_t::dealer_bust:        state = "dealer bust"; break;
    case game_state_t::player_won:         state = "player won"; break;
    case game_state_t::dealer_won:         state = "dealer won"; break;
    default:                               state = "unknown"; break;
  }
  ImGui::Text("Game state: %s", state.c_str());
  ImGui::SameLine();
}

static void gui_draw_scores(const std::vector<int> &scores) {
  std::string score_str = "";
  for (size_t i = 0; i < scores.size(); ++i)
    if (i == 0)
      score_str = std::to_string(scores[i]);
    else if (i != scores.size() - 1)
      score_str += ", " + std::to_string(scores[i]);
    else
      score_str += " or " + std::to_string(scores[i]);
  ImGui::Text("%s", score_str.c_str());
}

static void draw_hand(const deck_t &deck, float x, float y, float xoff) {
  float x_draw = x;
  for (const card_t &card : deck) {
    c->draw(card, x_draw, y, projection_mat);
    x_draw += xoff;
  }
}

static void gui_draw_cards_and_stats() {
  ImGui::Text("Money: %d", money);
  ImGui::Text("Bet: %d", bet);

  ImGui::Text("Dealer's hand:");
  float x = 14, y = 109, xoff = 60;
  if (draw_all_dealer_cards) {
    ImGui::SameLine();
    gui_draw_scores(dealer_scores);
    draw_hand(dealer_hand, x, y, xoff);
  } else {
    ImGui::SameLine();
    gui_draw_scores(dealer_first_card_scores);
    float x_draw = x;
    c->draw(dealer_hand[0], x_draw, y, projection_mat);
    c->draw_down(x_draw + xoff, y, projection_mat);
  }

  ImGui::Text("\n\n\n\n"); // I know
  ImGui::Text("Your hand:");
  ImGui::SameLine();
  gui_draw_scores(player_scores);
  draw_hand(player_hand, 14, 109 + 100, 60);
  ImGui::Text("\n\n\n\n");
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
        if (bet > 0 && bet <= money) {
          money -= bet;
          game_state_deal_cards();
        }
      break;
    case game_state_t::dealing_cards: // TODO this needs to be removed
      gui_draw_cards_and_stats();
      break;
    case game_state_t::tie:
      draw_all_dealer_cards = true;
      gui_draw_cards_and_stats();
      ImGui::Text("Standoff");
      game_continue();
      break;
    case game_state_t::player_natural:
      gui_draw_cards_and_stats();
      ImGui::Text("You win by natural");
      game_continue();
      break;
    case game_state_t::dealer_natural:
      draw_all_dealer_cards = true;
      gui_draw_cards_and_stats();
      ImGui::Text("Dealer wins by natural");
      game_continue();
      break;
    case game_state_t::the_play:
      gui_draw_cards_and_stats();
      if (ImGui::Button("Hit"))
        game_hit();
      ImGui::SameLine();
      if (ImGui::Button("Stay"))
        game_dealer_plays();
      break;
    case game_state_t::player_bust:
      gui_draw_cards_and_stats();
      ImGui::Text("haha busted u bitch");
      game_continue();
      break;
    case game_state_t::dealer_plays: // TODO this need to be removed too
      draw_all_dealer_cards = true;
      gui_draw_cards_and_stats();
      ImGui::Text("dealer playing");
      break;
    case game_state_t::dealer_bust:
      draw_all_dealer_cards = true;
      gui_draw_cards_and_stats();
      ImGui::Text("dealer bust");
      game_continue();
      break;
    case game_state_t::player_won:
      draw_all_dealer_cards = true;
      gui_draw_cards_and_stats();
      ImGui::Text("player won");
      game_continue();
      break;
    case game_state_t::dealer_won:
      draw_all_dealer_cards = true;
      gui_draw_cards_and_stats();
      ImGui::Text("dealer won");
      game_continue();
      break;
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
  driver_init("blaqjaq pays 3 to 2", 700, 525);

  load();

  driver_main_loop(frame, update, key_event, mouse_motion_event
      , mouse_button_event);

  cleanup();

  driver_destroy();

  return 0;
}

