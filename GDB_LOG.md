# GDB log

One dated section per week. Paste the commands you used and the output you saw, then explain what it means in your own words. Pasted output with no explanation earns partial credit at most. Each partner writes their own section.

## Week 11 (Lab 11)

Partner name: [enter your name]

> This editor-only workspace has no terminal or GDB. The calculations below
> are derived from the source, not a claim that the commands were executed.
> Run the commands locally and add the actual GDB transcript before submission.

### Predict before you run

The bug should miss hands 2, 4, 5, 7, and 8. It only demotes an ace before
processing a later card, so an over-21 total on the final card is never
corrected. It also demotes at most one ace at each step. For hand 4 (A, 6, 5),
it returns 22 instead of 12.

### Build and run (before the fix)

```
cmake -S . -B build
cmake --build build --target hand_bug
./build/hand_bug
```

Expected mismatches from the original buggy implementation (hand number,
printed total, correct total): 2 (22, 12), 4 (22, 12), 5 (23, 13), 7 (26,
16), and 8 (31, 21).

After the fix, the expected totals for hands 1-10 are 21, 12, 21, 12, 13, 21,
16, 21, 25, and 21. These expected values still need to be confirmed by
running `./build/hand_bug`.

### The GDB session on `Hand::value`

Commands to run against the original buggy version, in order (for the
three-card failing hand 4):

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
the end of `value()` and reports its return value. The original source has
since been fixed, so run this session from the pre-fix version to reproduce the
failure.

- Expected pre-fix values: after A, total 11 and aces-as-eleven 1; after 6,
  total 17 and aces-as-eleven 1; after 5, total 22 and aces-as-eleven 1.
- Expected `print *this`: the three stored cards A, 6, 5 and `m_count == 3`.
- Expected pre-fix `finish`: 22; correct value from the rules: 12.

### Cause and fix

- The line that was wrong: the `if (total > 21 ...)` check at the start of the
  loop examined the total before adding the current card, so the last card
  could push the hand over 21 without demoting an ace.
- The fix: demote an ace after each card is added while the total is over 21
  and an ace is still counted as 11.
- How it was checked: the hand tests assert A, A, 9 is 21 and A, 6, 5 is 12,
  along with hard totals, bust, and natural blackjack. The GDB session and
  post-fix run still need to be performed in a terminal.
