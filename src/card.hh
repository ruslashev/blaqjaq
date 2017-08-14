#pragma once

#include <vector>

enum suit_t {
    hearts
  , diamonds
  , clubs
  , spades
};

enum rank_t {
    two
  , three
  , four
  , five
  , six
  , seven
  , eight
  , nine
  , ten
  , jack
  , queen
  , king
  , ace
};

void print_suit(suit_t suit);
void print_rank(rank_t rank);

class card_t {
public:
  suit_t suit;
  rank_t rank;

  card_t(suit_t n_suit, rank_t n_rank);
  void print();
};

typedef std::vector<card_t> deck_t;

deck_t generate_deck(int packs);
void shuffle_deck(deck_t *deck);

