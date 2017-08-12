#pragma once

#include "ogl.hh"

class card_drawer {
  shader_program _sp;
  vertex_array_object _vao;
  array_buffer _vbo;
  element_array_buffer _ebo;
  GLint _vertex_pos_attr, _mvp_mat_unif;
  int _num_elements;
public:
  card_drawer();
  void draw(float x, float y, const glm::mat4 &projection);
};

