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

card_model::card_model()
  : _sp(shaders::simple_vert, shaders::simple_frag)
  , _vertex_pos_attr(_sp.bind_attrib("vertex_pos"))
  , _mvp_mat_unif(_sp.bind_uniform("mvp")) {
  _vao.bind();
  _vbo.bind();
  _ebo.bind();
  int tesselation = 15;
  float rounding = 0.65f, r = rounding; // alias
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

void card_model::draw(float x, float y, const glm::mat4 &projection) {
  _vao.bind();
  _sp.use_this_prog();

  float scale = 24; // 6
  glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(x, y, 0.f))
    , model = glm::scale(translate, glm::vec3(scale))
    , mvp = projection * model;

  glUniformMatrix4fv(_mvp_mat_unif, 1, GL_FALSE, glm::value_ptr(mvp));
  glDrawElements(GL_TRIANGLES, _num_elements, GL_UNSIGNED_SHORT, 0);
  _vao.unbind();
}

symbol_model::symbol_model(const char *filename)
  : _sp(shaders::simple_vert, shaders::black_frag)
  , _vertex_pos_attr(_sp.bind_attrib("vertex_pos"))
  , _mvp_mat_unif(_sp.bind_uniform("mvp")) {
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

void symbol_model::draw(const glm::mat4 &card_mvp) {
  _vao.bind();
  _sp.use_this_prog();

  float scale = 6, x = 1, y = 2;
  glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(x, y, 0.f))
    , model = glm::scale(translate, glm::vec3(scale))
    , mvp = card_mvp * model;

  glUniformMatrix4fv(_mvp_mat_unif, 1, GL_FALSE, glm::value_ptr(mvp));
  glDrawElements(GL_TRIANGLES, _num_elements, GL_UNSIGNED_SHORT, 0);
  _vao.unbind();
}

