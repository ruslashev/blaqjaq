#include "card_drawer.hh"
#include "shaders.hh"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

void card_model::_add_circle(std::vector<float> *vertices
    , std::vector<GLushort> *elements, float x, float y, float rounding
    , int tesselation) {
  int last_element = vertices->size() / 2;
  vertices->push_back(x);
  vertices->push_back(y);
  for (float th = 0; th <= 2.f * (float)M_PI; th += (2.f * (float)M_PI)
      / (float)tesselation) {
    float vx = x + cosf(th) * rounding, vy = y + sinf(th) * rounding;
    vertices->push_back(vx);
    vertices->push_back(vy);
  }
  for (int i = 0; i < tesselation; ++i) {
    elements->push_back(last_element);
    if (i != tesselation - 1)
      elements->push_back(last_element + 2 + i);
    else
      elements->push_back(last_element + 1);
    elements->push_back(last_element + 1 + i);
  }
}

card_model::card_model(float n_scale, int tesselation, float rounding)
  : _sp(shaders::simple_vert, shaders::unif_colored_mesh_frag)
  , _vertex_pos_attr(_sp.bind_attrib("vertex_pos"))
  , _mvp_mat_unif(_sp.bind_uniform("mvp"))
  , _vertex_color_unif(_sp.bind_uniform("vertex_color_unif"))
  , _scale(n_scale) {
  _vao.bind();
  _vbo.bind();
  _ebo.bind();
  const float r = rounding; // alias
  std::vector<float> vertices = {
    r,     r,
    0,     r,
    0,     7 - r,
    r,     7 - r,
    r,     7,
    5 - r, 7,
    5 - r, 7 - r,
    5,     7 - r,
    5,     r,
    5 - r, r,
    5 - r, 0,
    r,     0,
  };
  std::vector<GLushort> elements = {
     0,  1,  2,
     0,  2,  3,
    10, 11,  4,
    10,  4,  5,
     8,  9,  6,
     8,  6,  7
  };
  _add_circle(&vertices, &elements,     r,     r, rounding, tesselation);
  _add_circle(&vertices, &elements,     r, 7 - r, rounding, tesselation);
  _add_circle(&vertices, &elements, 5 - r, 7 - r, rounding, tesselation);
  _add_circle(&vertices, &elements, 5 - r,     r, rounding, tesselation);
  _vbo.upload(vertices);
  _ebo.upload(elements);
  _num_elements = elements.size();
  glEnableVertexAttribArray(_vertex_pos_attr);
  glVertexAttribPointer(_vertex_pos_attr, 2, GL_FLOAT, GL_FALSE
      , 2 * sizeof(float), 0);
  _vao.unbind();
  _vbo.unbind();
  _ebo.unbind();
  glDisableVertexAttribArray(_vertex_pos_attr);
}

void card_model::draw(bool down, const glm::mat4 &recv_mvp) {
  _vao.bind();
  _sp.use_this_prog();

  glm::mat4 model = glm::scale(glm::mat4(), glm::vec3(_scale))
    , mvp = recv_mvp * model;

  glUniformMatrix4fv(_mvp_mat_unif, 1, GL_FALSE, glm::value_ptr(mvp));
  if (down)
    glUniform3f(_vertex_color_unif, 214 / 255.f, 111 / 255.f, 169 / 255.f);
  else
    glUniform3f(_vertex_color_unif, 0.98f, 0.98f, 0.98f);
  glDrawElements(GL_TRIANGLES, _num_elements, GL_UNSIGNED_SHORT, 0);
  _vao.unbind();
}

symbol_model::symbol_model(const char *filename, float n_xoff
    , float n_yoff)
  : _sp(shaders::simple_vert, shaders::unif_colored_mesh_frag)
  , _vertex_pos_attr(_sp.bind_attrib("vertex_pos"))
  , _mvp_mat_unif(_sp.bind_uniform("mvp"))
  , _vertex_color_unif(_sp.bind_uniform("vertex_color_unif"))
  , _xoff(n_xoff)
  , _yoff(n_yoff) {
  std::ifstream ifs(filename, std::ios::in);
  if (!ifs)
    die("failed to open \"%s\"", filename);

  std::vector<float> vertices;
  std::vector<GLushort> elements;
  std::string token;
  while (ifs) {
    ifs >> token;
    if (!ifs)
      break;
    if (token == "v") {
      float x, y;
      ifs >> x;
      ifs >> y;
      vertices.push_back(x);
      vertices.push_back(y);
    } else if (token == "f") {
      GLushort a, b, c;
      ifs >> a;
      ifs >> b;
      ifs >> c;
      elements.push_back(a - 1);
      elements.push_back(b - 1);
      elements.push_back(c - 1);
    }
  }
  ifs.close();

  _vao.bind();
  _vbo.bind();
  _ebo.bind();
  _vbo.upload(vertices);
  _ebo.upload(elements);
  _num_elements = elements.size();
  glEnableVertexAttribArray(_vertex_pos_attr);
  glVertexAttribPointer(_vertex_pos_attr, 2, GL_FLOAT, GL_FALSE
      , 2 * sizeof(float), 0);
  _vao.unbind();
  _vbo.unbind();
  _ebo.unbind();
  glDisableVertexAttribArray(_vertex_pos_attr);
}

void symbol_model::draw(const glm::mat4 &recv_mvp, float scale, bool red) {
  _vao.bind();
  _sp.use_this_prog();

  glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(_xoff, _yoff
        , 0.f))
    , model = glm::scale(translate, glm::vec3(scale))
    , mvp = recv_mvp * model;

  glUniformMatrix4fv(_mvp_mat_unif, 1, GL_FALSE, glm::value_ptr(mvp));
  if (red)
    glUniform3f(_vertex_color_unif, 1, 0, 0);
  else
    glUniform3f(_vertex_color_unif, 0, 0, 0);
  glDrawElements(GL_TRIANGLES, _num_elements, GL_UNSIGNED_SHORT, 0);
  _vao.unbind();
}

card_drawer::card_drawer(float scale, int tesselation, float rounding
    , float symbols_scale_mult) {
  _scale = scale;
  _card = new card_model(_scale, tesselation, rounding);
  _symbols_scale = _scale * symbols_scale_mult;
  const float ss = _symbols_scale; // alias
  _ranks[rank_t::ace]   = new symbol_model("res/ace.obj",   ss * 0.05f, ss * 0.41f);
  _ranks[rank_t::two]   = new symbol_model("res/two.obj",   ss * 0.07f, ss * 0.42f);
  _ranks[rank_t::three] = new symbol_model("res/three.obj", ss * 0.06f, ss * 0.42f);
  _ranks[rank_t::four]  = new symbol_model("res/four.obj",  ss * 0.05f, ss * 0.42f);
  _ranks[rank_t::five]  = new symbol_model("res/five.obj",  ss * 0.07f, ss * 0.43f);
  _ranks[rank_t::six]   = new symbol_model("res/six.obj",   ss * 0.05f, ss * 0.42f);
  _ranks[rank_t::seven] = new symbol_model("res/seven.obj", ss * 0.07f, ss * 0.43f);
  _ranks[rank_t::eight] = new symbol_model("res/eight.obj", ss * 0.05f, ss * 0.42f);
  _ranks[rank_t::nine]  = new symbol_model("res/nine.obj",  ss * 0.06f, ss * 0.42f);
  _ranks[rank_t::ten]   = new symbol_model("res/ten.obj",   ss * 0.05f, ss * 0.42f);
  _ranks[rank_t::jack]  = new symbol_model("res/jack.obj",  ss * 0.12f, ss * 0.43f);
  _ranks[rank_t::queen] = new symbol_model("res/queen.obj", ss * 0.05f, ss * 0.48f);
  _ranks[rank_t::king]  = new symbol_model("res/king.obj",  ss * 0.07f, ss * 0.43f);
  _suits[suit_t::hearts]   = new symbol_model("res/heart.obj",   ss * 0.05f, ss * 0.83f);
  _suits[suit_t::diamonds] = new symbol_model("res/diamond.obj", ss * 0.07f, ss * 0.8f);
  _suits[suit_t::clubs]    = new symbol_model("res/club.obj",    ss * 0.05f, ss * 0.81f);
  _suits[suit_t::spades]   = new symbol_model("res/spade.obj",   ss * 0.06f, ss * 0.8f);
}

void card_drawer::draw(const card_t &card, float x, float y
    , const glm::mat4 &projection) {
  glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(x, y, 0.f))
    , mvp = projection * translate
    , flip_mvp = projection * translate
      * glm::rotate(glm::mat4(), (float)M_PI, glm::vec3(0, 0, 1))
      * glm::translate(glm::mat4(), glm::vec3(-5.f * _scale, -7.f * _scale, 0));

  _card->draw(false, mvp);
  bool red = card.suit == suit_t::hearts || card.suit == suit_t::diamonds;
  _ranks[card.rank]->draw(mvp, _symbols_scale, red);
  _suits[card.suit]->draw(mvp, _symbols_scale, red);
  _ranks[card.rank]->draw(flip_mvp, _symbols_scale, red);
  _suits[card.suit]->draw(flip_mvp, _symbols_scale, red);
}

void card_drawer::draw_down(float x, float y, const glm::mat4 &projection) {
  glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(x, y, 0.f))
    , mvp = projection * translate;
  _card->draw(true, mvp);
}

