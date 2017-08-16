#include "card.hh"
#include "utils.hh"
#include <cstdio>
#include <cassert>
#include <cmath>

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

card_t::card_t(rank_t n_rank, suit_t n_suit)
  : suit(n_suit)
  , rank(n_rank) {
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
  assert(packs >= 0);
  for (int i = 0; i < packs; ++i)
    for (int suit = suit_t::hearts; suit <= suit_t::hearts + 3; ++suit)
      for (int rank = rank_t::two; rank <= rank_t::two + 12; ++rank)
        deck.emplace_back((suit_t)suit, (rank_t)rank);
  return deck;
}

static int rand_in_range(int min, int max) {
  return min + rand() % (max - min + 1);
}

void shuffle_deck(deck_t *deck) {
  for (size_t i = 0; i < deck->size(); ++i)
    std::swap((*deck)[i], (*deck)[rand_in_range(0, deck->size() - 1)]);
}

struct score_bin_tree {
  int score;
  score_bin_tree *left, *right;
  score_bin_tree(int n_score) : score(n_score), left(nullptr), right(nullptr) {}
};

static void count_scores_aux(score_bin_tree *node, const deck_t &cards
    , int index) {
  if (index < 0 || index > (int)cards.size() - 1)
    return;
  card_t head = cards[index];
  if (head.rank != rank_t::ace) {
    switch (head.rank) {
      case rank_t::two:   node->score += 2;  break;
      case rank_t::three: node->score += 3;  break;
      case rank_t::four:  node->score += 4;  break;
      case rank_t::five:  node->score += 5;  break;
      case rank_t::six:   node->score += 6;  break;
      case rank_t::seven: node->score += 7;  break;
      case rank_t::eight: node->score += 8;  break;
      case rank_t::nine:  node->score += 9;  break;
      case rank_t::ten:   node->score += 10; break;
      case rank_t::jack:  node->score += 10; break;
      case rank_t::queen: node->score += 10; break;
      case rank_t::king:  node->score += 10; break;
      default: die("things that shouldn't happen for 400");
    }
    count_scores_aux(node, cards, index + 1);
  } else {
    node->left = new score_bin_tree(1 + node->score);
    count_scores_aux(node->left, cards, index + 1);
    node->right = new score_bin_tree(11 + node->score);
    count_scores_aux(node->right, cards, index + 1);
  }
}

static void gather_unique_leaves(score_bin_tree *branch
    , std::vector<int> *collection) {
  if (branch->left)
    gather_unique_leaves(branch->left, collection);
  if (branch->right)
    gather_unique_leaves(branch->right, collection);
  if (!branch->left && !branch->right) {
    for (int e : *collection)
      if (e == branch->score)
        return;
    collection->push_back(branch->score);
  }
}

std::vector<int> count_scores(const deck_t &deck) {
  score_bin_tree root(0);
  count_scores_aux(&root, deck, 0);
  std::vector<int> scores;
  gather_unique_leaves(&root, &scores);
  return scores;
}

card_t random_card() {
  return card_t((suit_t)(suit_t::hearts + rand_in_range(0, 3))
      , (rank_t)(rank_t::two + rand_in_range(0, 12)));
}

deck_t deal_hand() {
  deck_t hand;
  hand.push_back(random_card());
  hand.push_back(random_card());
  return hand;
}

bool scores_have_bj(const std::vector<int> &scores) {
  for (int score : scores)
    if (score == 21)
      return true;
  return false;
}

