#include "blackjack.h"

#include <iostream>
#include <random>
#include <string>

#include "deck.h"
#include "hand.h"
#include "input.h"

namespace {

inline constexpr int starting_bankroll{10000};
inline constexpr int bet_amount{1000};
inline constexpr int reshuffle_threshold{15};

void print_money(int cents) {
  std::cout << cents / 100 << '.';
  if (cents % 100 < 10) {
    std::cout << '0';
  }
  std::cout << cents % 100;
}

Card deal_card(Deck &deck, std::mt19937 &gen) {
  if (deck.empty()) {
    deck.shuffle(gen);
  }
  return deck.deal();
}

} // namespace

void play_blackjack() {
  std::random_device device{};
  std::mt19937 gen{device()};
  Deck deck;
  deck.shuffle(gen);
  int bankroll{starting_bankroll};

  std::cout << "\n--- Blackjack ---\n"
            << "You start with $100. Each round bets $10. Blackjack pays "
               "3:2; dealer stands on soft 17.\n";

  bool quit{false};
  while (bankroll > 0) {
    if (bankroll < bet_amount) {
      std::cout << "You have less than the fixed $10 bet and cannot continue.\n";
      break;
    }

    std::string answer;
    if (!prompt_line("Press Enter for the next round, or q to quit: ",
                     answer)) {
      break;
    }
    if (answer == "q" || answer == "Q") {
      break;
    }
    if (deck.remaining() < reshuffle_threshold) {
      deck.shuffle(gen);
      std::cout << "Shuffling the deck.\n";
    }

    bankroll -= bet_amount;
    Hand player;
    Hand dealer;

    player += deal_card(deck, gen);
    dealer += deal_card(deck, gen);
    player += deal_card(deck, gen);
    dealer += deal_card(deck, gen);

    std::cout << "Your hand: " << player << " (" << player.value() << ")\n"
              << "Dealer shows: " << dealer[0] << " [hidden]\n";

    if (player.is_blackjack() || dealer.is_blackjack()) {
      std::cout << "Dealer hand: " << dealer << " (" << dealer.value()
                << ")\n";
      if (player.is_blackjack() && dealer.is_blackjack()) {
        bankroll += bet_amount;
        std::cout << "Push. Your bet is returned.\n";
      } else if (player.is_blackjack()) {
        bankroll += bet_amount + bet_amount * 3 / 2;
        std::cout << "Blackjack! You win $15.\n";
      } else {
        std::cout << "Dealer has Blackjack. You lose $10.\n";
      }
      continue;
    }

    bool player_stands{false};
    while (!player_stands && !player.is_bust() &&
           player.count() < max_hand) {
      std::string action;
      if (!prompt_line("Hit, stand, or quit? (h/s/q): ", action)) {
        bankroll += bet_amount;
        quit = true;
        break;
      }
      if (action == "h" || action == "H") {
        player += deal_card(deck, gen);
        std::cout << "Your hand: " << player << " (" << player.value()
                  << ")\n";
      } else if (action == "s" || action == "S") {
        player_stands = true;
      } else if (action == "q" || action == "Q") {
        bankroll += bet_amount;
        quit = true;
        break;
      } else {
        std::cout << "Enter h, s, or q.\n";
      }
    }

    if (quit) {
      break;
    }

    if (player.is_bust()) {
      std::cout << "Bust. You lose $10.\n";
      continue;
    }

    std::cout << "Dealer hand: " << dealer;
    while (dealer.value() < 17 && dealer.count() < max_hand) {
      dealer += deal_card(deck, gen);
    }
    std::cout << " -> " << dealer << " (" << dealer.value() << ")\n";

    if (dealer.is_bust() || player.value() > dealer.value()) {
      bankroll += bet_amount * 2;
      std::cout << "You win $10.\n";
    } else if (player.value() < dealer.value()) {
      std::cout << "Dealer wins. You lose $10.\n";
    } else {
      bankroll += bet_amount;
      std::cout << "Push. Your bet is returned.\n";
    }
  }

  std::cout << "Final bankroll: $";
  print_money(bankroll);
  std::cout << (bankroll == 0 ? ". You are out of money.\n"
                              : ". Thanks for playing.\n");
}
