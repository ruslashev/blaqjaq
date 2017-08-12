#include "card_drawer.hh"
#include "shaders.hh"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

card_drawer::card_drawer()
  : _sp(shaders::simple_vert, shaders::simple_frag)
  , _vertex_pos_attr(_sp.bind_attrib("vertex_pos"))
  , _mvp_mat_unif(_sp.bind_uniform("mvp")) {
  _vao.bind();
  _vbo.bind();
  _ebo.bind();
  std::vector<float> vertices = {
    5, 7,
    0, 7,
    0, 0,
    5, 0
  };
  std::vector<GLushort> elements = {
    0, 1, 2, 3
  };
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

void card_drawer::draw(float x, float y, const glm::mat4 &projection) {
  _vao.bind();
  _sp.use_this_prog();

  float scale = 6;
  glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(x, y, 0.f))
    , model = glm::scale(translate, glm::vec3(scale))
    , mvp = projection * model;

  glUniformMatrix4fv(_mvp_mat_unif, 1, GL_FALSE, glm::value_ptr(mvp));
  glDrawElements(GL_TRIANGLE_FAN, _num_elements, GL_UNSIGNED_SHORT, 0);
  _vao.unbind();
}

