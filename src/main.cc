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
static bool wireframe = false;
static deck_t deck;

static void load() {
  glClearColor(187 / 255.f, 206 / 255.f, 242 / 255.f, 1.f);

  deck = generate_deck(1);

  c = new card_drawer;
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

  float xoff = 50, yoff = 70;
  float x = 10, y = 10;
  for (const card_t &card : deck)
    if (card.suit == suit_t::hearts) {
      c->draw(card, x, y, projection_mat);
      x += xoff;
    }
  x = 10;
  y += yoff;
  for (const card_t &card : deck)
    if (card.suit == suit_t::clubs) {
      c->draw(card, x, y, projection_mat);
      x += xoff;
    }
  x = 10;
  y += yoff;
  for (const card_t &card : deck)
    if (card.suit == suit_t::spades) {
      c->draw(card, x, y, projection_mat);
      x += xoff;
    }
  x = 10;
  y += yoff;
  for (const card_t &card : deck)
    if (card.suit == suit_t::diamonds) {
      c->draw(card, x, y, projection_mat);
      x += xoff;
    }
  x = 10;
  y += yoff;
  for (int i = 0; i < 13; ++i) {
    c->draw_down(x, y, projection_mat);
    x += xoff;
  }
}

static void cleanup() {
  delete c;
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

