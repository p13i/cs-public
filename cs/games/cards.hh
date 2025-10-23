#ifndef CS_GAMES_CARDS_HH
#define CS_GAMES_CARDS_HH

#include <string>

#include "cs/util/fmt.hh"

namespace cs::games {
struct Card {
  char rank;
  char suite;

  std::string ToString(const Card& card);
};

static Card CARDS[] = {
    // clang-format off
  // Spades:
  Card{.rank = 'A', .suite = 'S'},
  Card{.rank = '2', .suite = 'S'},
  Card{.rank = '3', .suite = 'S'},
  Card{.rank = '4', .suite = 'S'},
  Card{.rank = '5', .suite = 'S'},
  Card{.rank = '6', .suite = 'S'},
  Card{.rank = '7', .suite = 'S'},
  Card{.rank = '8', .suite = 'S'},
  Card{.rank = '9', .suite = 'S'},
  Card{.rank = 'T', .suite = 'S'},
  Card{.rank = 'J', .suite = 'S'},
  Card{.rank = 'Q', .suite = 'S'},
  Card{.rank = 'K', .suite = 'S'},

  // Hearts:
  Card{.rank = 'A', .suite = 'H'},
  Card{.rank = '2', .suite = 'H'},
  Card{.rank = '3', .suite = 'H'},
  Card{.rank = '4', .suite = 'H'},
  Card{.rank = '5', .suite = 'H'},
  Card{.rank = '6', .suite = 'H'},
  Card{.rank = '7', .suite = 'H'},
  Card{.rank = '8', .suite = 'H'},
  Card{.rank = '9', .suite = 'H'},
  Card{.rank = 'T', .suite = 'H'},
  Card{.rank = 'J', .suite = 'H'},
  Card{.rank = 'Q', .suite = 'H'},
  Card{.rank = 'K', .suite = 'H'},

  // Clubs:
  Card{.rank = 'A', .suite = 'C'},
  Card{.rank = '2', .suite = 'C'},
  Card{.rank = '3', .suite = 'C'},
  Card{.rank = '4', .suite = 'C'},
  Card{.rank = '5', .suite = 'C'},
  Card{.rank = '6', .suite = 'C'},
  Card{.rank = '7', .suite = 'C'},
  Card{.rank = '8', .suite = 'C'},
  Card{.rank = '9', .suite = 'C'},
  Card{.rank = 'T', .suite = 'C'},
  Card{.rank = 'J', .suite = 'C'},
  Card{.rank = 'Q', .suite = 'C'},
  Card{.rank = 'K', .suite = 'C'},

  // Diamonds:
  Card{.rank = 'A', .suite = 'D'},
  Card{.rank = '2', .suite = 'D'},
  Card{.rank = '3', .suite = 'D'},
  Card{.rank = '4', .suite = 'D'},
  Card{.rank = '5', .suite = 'D'},
  Card{.rank = '6', .suite = 'D'},
  Card{.rank = '7', .suite = 'D'},
  Card{.rank = '8', .suite = 'D'},
  Card{.rank = '9', .suite = 'D'},
  Card{.rank = 'T', .suite = 'D'},
  Card{.rank = 'J', .suite = 'D'},
  Card{.rank = 'Q', .suite = 'D'},
  Card{.rank = 'K', .suite = 'D'}
    // clang-format on
};

std::string Card::ToString(const Card& card) {
  std::string rank = "invalid";
  std::string suite = "invalid";

  // clang-format off
  // Ranks:
  switch (card.rank) {
    case 'A': rank = "Ace"; break;
    case '2': rank = "Two"; break;
    case '3': rank = "Three"; break;
    case '4': rank = "Four"; break;
    case '5': rank = "Five"; break;
    case '6': rank = "Six"; break;
    case '7': rank = "Seven"; break;
    case '8': rank = "Eight"; break;
    case '9': rank = "Nine"; break;
    case 'T': rank = "Ten"; break;
    case 'J': rank = "Jack"; break;
    case 'Q': rank = "Queen"; break;
    case 'K': rank = "King"; break;
  }

  // Suites:
  switch (card.suite) {
    case 'S': suite = "Spades"; break;
    case 'H': suite = "Hearts"; break;
    case 'C': suite = "Clubs"; break;
    case 'D': suite = "Diamonds"; break;
  }
  // clang-format on
  return FMT("%s of %s", rank, suite);
}

}  // namespace cs::games

#endif  // CS_GAMES_CARDS_HH
