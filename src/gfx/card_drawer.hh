#pragma once

#include "ogl.hh"
#include "../card.hh"
#include <map>

class card_model {
  shader_program _sp;
  vertex_array_object _vao;
  array_buffer _vbo;
  element_array_buffer _ebo;
  GLint _vertex_pos_attr, _mvp_mat_unif, _vertex_color_unif;
  int _num_elements;
  float _scale;

  void _add_circle(std::vector<float> *vertices, std::vector<GLushort> *elements
      , float x, float y, float rounding, int tesselation);
public:
  card_model(float n_scale, int tesselation, float rounding);
  void draw(bool down, const glm::mat4 &recv_mvp);
};

class symbol_model {
  shader_program _sp;
  vertex_array_object _vao;
  array_buffer _vbo;
  element_array_buffer _ebo;
  GLint _vertex_pos_attr, _mvp_mat_unif, _vertex_color_unif;
  int _num_elements;
  float _xoff, _yoff;
public:
  symbol_model(const char *filename, float n_xoff, float n_yoff);
  void draw(const glm::mat4 &recv_mvp, float scale, bool red);
};

class card_drawer {
  card_model *_card;
  std::map<rank_t, symbol_model*> _ranks;
  std::map<suit_t, symbol_model*> _suits;
  // std::map<rank_t, float> _suit_xoffset_for_rank;
  float _scale, _symbols_scale;
public:
  card_drawer();
  void draw(const card_t &card, float x, float y, const glm::mat4 &projection);
  void draw_down(float x, float y, const glm::mat4 &projection);
};

