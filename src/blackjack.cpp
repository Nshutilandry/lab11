#include "blackjack.h"

#include <iostream>
#include <random>
#include <string>

#include "deck.h"
#include "hand.h"
#include "input.h"

namespace {

bool parse_money(const std::string &text, int &amount) {
  const std::string::size_type dot{text.find('.')};
  const std::string whole_text{text.substr(0, dot)};
  int dollars{0};
  if (!parse_int(whole_text, dollars) || dollars < 0) {
    return false;
  }

  int cents{0};
  if (dot != std::string::npos) {
    const std::string fraction{text.substr(dot + 1)};
    if (fraction.empty() || fraction.size() > 2) {
      return false;
    }
    for (char digit : fraction) {
      if (digit < '0' || digit > '9') {
        return false;
      }
      cents = cents * 10 + (digit - '0');
    }
    if (fraction.size() == 1) {
      cents *= 10;
    }
  }

  const long long total{static_cast<long long>(dollars) * 100 + cents};
  if (total > 1000000000) {
    return false;
  }
  amount = static_cast<int>(total);
  return true;
}

void print_money(int cents) {
  std::cout << cents / 100 << '.';
  if (cents % 100 < 10) {
    std::cout << '0';
  }
  std::cout << cents % 100;
}

bool get_bet(int bankroll, int &bet) {
  std::string line;
  while (true) {
    std::cout << "Bankroll: $";
    print_money(bankroll);
    if (!prompt_line(" | Bet ($; or q to quit): ",
                     line)) {
      return false;
    }
    if (line == "q" || line == "Q") {
      return false;
    }
    if (parse_money(line, bet) && bet > 0 && bet <= bankroll) {
      return true;
    }
    std::cout << "Enter a bet from $0.01 up to your bankroll.\n";
  }
}

} // namespace

void play_blackjack() {
  std::random_device device{};
  std::mt19937 gen{device()};
  int bankroll{10000};
  bool quit{false};

  std::cout << "\n--- Blackjack ---\n"
            << "You start with $100. Blackjack pays 3:2, rounded to the "
               "nearest cent; dealer stands on 17.\n";

  while (bankroll > 0 && !quit) {
    int wager{0};
    if (!get_bet(bankroll, wager)) {
      break;
    }
    bankroll -= wager;

    Deck deck;
    deck.shuffle(gen);
    Hand player;
    Hand dealer;

    player += deck.deal();
    dealer += deck.deal();
    player += deck.deal();
    dealer += deck.deal();

    std::cout << "Your hand: " << player << " (" << player.value() << ")\n"
              << "Dealer shows: " << dealer[0] << " [hidden]\n";

    if (player.is_blackjack() || dealer.is_blackjack()) {
      std::cout << "Dealer hand: " << dealer << " (" << dealer.value()
                << ")\n";
      if (player.is_blackjack() && dealer.is_blackjack()) {
        bankroll += wager;
        std::cout << "Push. Your bet is returned.\n";
      } else if (player.is_blackjack()) {
        bankroll += wager + (wager * 3 + 1) / 2;
        std::cout << "Blackjack! You win 3:2.\n";
      } else {
        std::cout << "Dealer has Blackjack. You lose this bet.\n";
      }
      continue;
    }

    bool player_stands{false};
    while (!player_stands && !player.is_bust() &&
           player.count() < max_hand) {
      std::string action;
      if (!prompt_line("Hit, stand, double, or quit? (h/s/d/q): ", action)) {
        bankroll += wager;
        quit = true;
        break;
      }
      if (action == "h" || action == "H") {
        player += deck.deal();
        std::cout << "Your hand: " << player << " (" << player.value()
                  << ")\n";
      } else if (action == "s" || action == "S") {
        player_stands = true;
      } else if (action == "d" || action == "D") {
        if (player.count() != 2 || bankroll < wager) {
          std::cout << "Double is available only on your first two cards "
                       "when you can cover the additional bet.\n";
          continue;
        }
        bankroll -= wager;
        wager *= 2;
        player += deck.deal();
        std::cout << "Your hand: " << player << " (" << player.value()
                  << ")\n";
        player_stands = true;
      } else if (action == "q" || action == "Q") {
        bankroll += wager;
        quit = true;
      } else {
        std::cout << "Enter h, s, d, or q.\n";
      }
    }
    if (quit) {
      break;
    }

    if (player.is_bust()) {
      std::cout << "Bust. You lose $";
      print_money(wager);
      std::cout << ".\n";
      continue;
    }

    std::cout << "Dealer hand: " << dealer;
    while (dealer.value() < 17 && dealer.count() < max_hand) {
      dealer += deck.deal();
    }
    std::cout << " -> " << dealer << " (" << dealer.value() << ")\n";

    if (dealer.is_bust() || player.value() > dealer.value()) {
      bankroll += wager * 2;
      std::cout << "You win $";
      print_money(wager);
      std::cout << ".\n";
    } else if (player.value() < dealer.value()) {
      std::cout << "Dealer wins. You lose this bet.\n";
    } else {
      bankroll += wager;
      std::cout << "Push. Your bet is returned.\n";
    }
  }

  std::cout << "Final bankroll: $";
  print_money(bankroll);
  std::cout << (bankroll == 0 ? ". You are out of money.\n"
                              : ". Thanks for playing.\n");
}
