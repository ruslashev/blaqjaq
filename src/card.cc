#include "card.hh"
#include <cstdio>

void print_suit(suit_t suit) {
  switch (suit) {
    case hearts:   printf("♥"); break;
    case diamonds: printf("♦"); break;
    case clubs:    printf("♣"); break;
    case spades:   printf("♠"); break;
  }
}

void print_rank(rank_t rank) {
  switch (rank) {
    case ace:   printf("A"); break;
    case two:   printf("2"); break;
    case three: printf("3"); break;
    case four:  printf("4"); break;
    case five:  printf("5"); break;
    case six:   printf("6"); break;
    case seven: printf("7"); break;
    case eight: printf("8"); break;
    case nine:  printf("9"); break;
    case ten:   printf("t"); break;
    case jack:  printf("J"); break;
    case queen: printf("Q"); break;
    case king:  printf("K"); break;
  }
}

card_t::card_t(suit_t n_suit, rank_t n_rank)
  : suit(n_suit)
  , rank(n_rank) {
}

void card_t::print() {
  print_suit(suit);
  print_rank(rank);
  puts("");
}

deck_t generate_deck(int packs) {
  deck_t deck;
  for (int suit = suit_t::hearts; suit <= suit_t::hearts + 3; ++suit)
    for (int rank = rank_t::ace; rank <= rank_t::ace + 12; ++rank)
      deck.emplace_back((suit_t)suit, (rank_t)rank);
  return deck;
}

