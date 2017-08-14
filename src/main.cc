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
static card_drawer *big_cd, *small_cd;
static bool wireframe = false;
static deck_t deck, shuffled_deck;

static void load() {
  glClearColor(187 / 255.f, 206 / 255.f, 242 / 255.f, 1.f);

  deck = generate_deck(1);
  shuffled_deck = generate_deck(1);
  shuffle_deck(&shuffled_deck);

  big_cd = new card_drawer(20.f, 15, 0.62f, 3.3f);
  small_cd = new card_drawer(10.f, 15, 0.62f, 3.3f);
}

static void key_event(char key, bool down) {
  if (key == 'f' && down)
    wireframe = !wireframe;
}

static void mouse_motion_event(float xrel, float yrel, int x, int y) {
}

static void mouse_button_event(int button, bool down, int x, int y) {
}

static void update(double dt, double t) {
}

static void frame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  if (0 && ImGui::BeginMainMenuBar()) {
    if (ImGui::SmallButton("Show test window"))
      show_test_window = !show_test_window;
    ImGui::EndMainMenuBar();
  }
  if (show_test_window) {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_FirstUseEver);
    ImGui::ShowTestWindow(&show_test_window);
  }

  projection_mat = glm::ortho(0.f, (float)ImGui::GetIO().DisplaySize.x
      , (float)ImGui::GetIO().DisplaySize.y, 0.f, -1.f, 1.f);

  for (size_t i = 0; i < shuffled_deck.size(); ++i)
    small_cd->draw(shuffled_deck[i], 10 + i * 14, 10, projection_mat);

  float xstart = 10, ystart = 90, xoff = 110, yoff = 150;
  float x = xstart, y = ystart;
  for (const card_t &card : deck)
    if (card.suit == suit_t::hearts) {
      big_cd->draw(card, x, y, projection_mat);
      x += xoff;
    }
  x = xstart;
  y += yoff;
  for (const card_t &card : deck)
    if (card.suit == suit_t::clubs) {
      big_cd->draw(card, x, y, projection_mat);
      x += xoff;
    }
  x = xstart;
  y += yoff;
  for (const card_t &card : deck)
    if (card.suit == suit_t::spades) {
      big_cd->draw(card, x, y, projection_mat);
      x += xoff;
    }
  x = xstart;
  y += yoff;
  for (const card_t &card : deck)
    if (card.suit == suit_t::diamonds) {
      big_cd->draw(card, x, y, projection_mat);
      x += xoff;
    }
  x = xstart;
  y += yoff;
  for (int i = 0; i < 13; ++i) {
    big_cd->draw_down(x, y, projection_mat);
    x += xoff;
  }
}

static void cleanup() {
  delete big_cd;
  delete small_cd;
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

