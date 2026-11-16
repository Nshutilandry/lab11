# Operators notes (Lab 11)

Fill this in as you work. Commit it with the code.

## 1. Test table for `Hand`

At least six hands. Work out each expected value from the rules **before** you run any code. At least one soft hand, one bust, and one blackjack must be in the table. Every row must also appear as a test in `tests/test_hand.cpp`.

| Hand | Expected `value()` | `is_bust()` | `is_blackjack()` |
| --- | --- | --- | --- |
| A, K | 21 | false | true |
| A, A | 12 | false | false |
| A, A, 9 | 21 | false | false |
| A, 6, 5 | 12 | false | false |
| 10, 10, 5 | 25 | true | false |
| 7, 7, 7 | 21 | false | false |
| A, 5 | 16 | false | false |

## 2. Design artifacts

The implementation and tests are in the existing files `src/card.cpp`,
`src/hand.cpp`, `src/blackjack.cpp`, and `tests/test_hand.cpp`. The diagrams
below are included here; no separate diagram files were added.

- Updated class diagram (with `Hand` and its relationship to `Card`):

  ```text
  Card
    rank: int
    suit: Suit
      ^
      | contains by value (composition, up to 11)
  Hand
    m_cards: Card[max_hand]
    m_count: int
    +=(Card), [](int), value(), is_blackjack(), is_bust()
  ```

- Flowchart for `Hand::value`:

  ```text
  Start -> total = 0, ace_count = 0
         -> for each card: add ace as 1 and count it;
                           add face card as 10;
                           otherwise add rank
         -> if there is an ace and total + 10 <= 21, add 10
         -> return total
  ```

- Flowchart for the dealer's turn:

  ```text
  Reveal dealer hand -> while total < 17 and hand has room
                         (soft 17 is already worth 17):
                         deal one card and add it
                      -> compare with player's non-busted total
                      -> dealer bust/player higher: player wins;
                         equal: push; otherwise dealer wins
  ```

  The game starts with $100, wagers a fixed $10 each round, and reshuffles
  before a round when fewer than 15 cards remain. A natural blackjack returns
  the original wager plus 3:2 winnings ($25 total for a $10 bet). Because the
  wager is fixed, play ends if the remaining bankroll is below $10.

## 3. Ownership classification

For each relationship, write **composition**, **aggregation**, or **association**, then one sentence that says why. Use the test questions from class: if the whole is destroyed, does the part go too? Could the same part belong to two wholes? Does the class only store a reference to something it does not own? A complete answer states what happens to the part when the whole is destroyed, and whether anything else can still hold the part.

| Relationship | Composition, aggregation, or association | One-sentence justification |
| --- | --- | --- |
| `Hand` and its `Card`s | Composition | A Hand stores its Card objects directly in its fixed array, so destroying the Hand destroys those Card subobjects and no other Hand can share them. |
| The Blackjack game and its `Deck` and `Hand`s | Composition | The game creates the Deck and Hands as local round objects, so they are destroyed when their scope ends and no other game owns those objects. |
| `Player` and the current `Room*` in the Dungeon | Association | The Room pointer is non-owning: destroying Player does not destroy the Room, which remains owned by the dungeon. |

## 4. Checklist before you push

- [ ] `cmake --build build` has no warnings
- [ ] `ctest` passes, including your own `Hand` tests
- [ ] A run under AddressSanitizer is clean (`-DENABLE_ASAN=ON`, separate build folder)
- [ ] `GDB_LOG.md`: each partner has written their own Week 11 section
- [ ] Commits use Conventional Commit messages, and both partners have commits
