#pragma once

#include "ogl.hh"

class card_drawer {
  shader_program _sp;
  vertex_array_object _vao;
  array_buffer _vbo;
  element_array_buffer _ebo;
  GLint _vertex_pos_attr, _mvp_mat_unif;
  int _num_elements;
  void _add_circle(std::vector<float> *vertices, std::vector<GLushort> *elements
      , float x, float y, float rounding, int tesselation);
public:
  card_drawer();
  void draw(float x, float y, const glm::mat4 &projection);
};

