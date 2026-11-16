# GDB log

One dated section per week. Paste the commands you used and the output you saw, then explain what it means in your own words. Pasted output with no explanation earns partial credit at most. Each partner writes their own section.

## Week 11 (Lab 11)

Partner name: [enter your name]

> This editor-only workspace has no terminal or GDB, so the session below is
> a reproducible procedure and expected result, not a claim that GDB was run.
> Run the commands and replace the expected values with your actual transcript.

### Predict before you run

The bug should miss hands 2, 4, 5, 7, and 8. It only demotes an ace before
processing a later card, so an over-21 total on the final card is never
corrected. It also demotes at most one ace at each step. For hand 4 (A, 6, 5),
it returns 22 instead of 12.

### Build and run

```
cmake -S . -B build
cmake --build build --target hand_bug
./build/hand_bug
```

Expected mismatches (hand number, printed total, correct total): 2 (22, 12),
4 (22, 12), 5 (23, 13), 7 (26, 16), and 8 (31, 21).

### The GDB session on `Hand::value`

Commands to run, in order (for the three-card failing hand 4):

```text
cmake -S . -B build
cmake --build build --target hand_bug
gdb ./build/hand_bug
(gdb) break Hand::value if m_count == 3
(gdb) run 4
(gdb) display total
(gdb) display aces_as_eleven
(gdb) next
...
(gdb) print *this
(gdb) finish
```

The breakpoint condition restricts the stop to three-card hands. `display`
reprints the two ace-calculation locals after each step; `next` advances one
source line; `print *this` shows the current cards and count; `finish` runs to
the end of `value()` and reports its return value.

- Expected values through hand 4's loop: after A, total 11 and aces-as-eleven
  1; after 6, total 17 and aces-as-eleven 1; after 5, total 22 and
  aces-as-eleven 1.
- Expected `print *this`: the three stored cards A, 6, 5 and `m_count == 3`.
- Expected `finish`: 22; correct value from the rules: 12.

### Cause and fix

- The line that is wrong: the `if (total > 21 ...)` check at the start of the
  loop. It checks the prior total before adding the current card, so it cannot
  demote an ace when the last card pushes the hand over 21.
- The change you made: compute the hard total with every ace counted as 1,
  then add 10 once if an ace can safely count as 11.
- How you checked it: the focused tests assert A, A, 9 is 21 and A, 6, 5 is
  12, along with hard totals, bust, and natural blackjack. The build/test run
  and actual GDB transcript still need to be done in a terminal.
