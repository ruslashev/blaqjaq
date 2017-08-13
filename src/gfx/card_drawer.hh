#pragma once

#include "ogl.hh"
#include <map>

class symbol_model {
  shader_program _sp;
  vertex_array_object _vao;
  array_buffer _vbo;
  element_array_buffer _ebo;
  GLint _vertex_pos_attr, _mvp_mat_unif;
  int _num_elements;
public:
  symbol_model(const char *filename);
  void draw(const glm::mat4 &card_mvp);
};

class card_model {
  shader_program _sp;
  vertex_array_object _vao;
  array_buffer _vbo;
  element_array_buffer _ebo;
  GLint _vertex_pos_attr, _mvp_mat_unif;
  int _num_elements;

  void _add_circle(std::vector<float> *vertices, std::vector<GLushort> *elements
      , float x, float y, float rounding, int tesselation);
public:
  card_model();
  void draw(float x, float y, const glm::mat4 &projection);
};

