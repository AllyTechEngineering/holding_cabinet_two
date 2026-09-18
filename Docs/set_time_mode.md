# Set Time Mode

Content owner for `SetTime-Decision`, `SetTime-Adjust`, `SetTime-Confirm`
(see `states_modes.md` for transitions/timeouts/heat-lifecycle rules —
this file covers screen content/wording and this screen's value bounds).


## SetTime-Decision

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | C | o | u | n | t | d | o | w | n | | T | i | m | e | r | |
| Row 2 | E | n | t | e | r | | Y | | M | o | d | e | | N | | |

As displayed:
```
Countdown Timer 
Enter Y Mode N  
```

## SetTime-Adjust

`HH:MM` is a live placeholder for the countdown duration, no seconds
field, no leading-zero padding beyond the fixed `HH:MM` shape (e.g.
`4:30`, `10:00`). Valid range: **min 0:30 (30 minutes), max 10:00 (10
hours)** — Up/Down has no effect past either bound.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | S | e | t | | T | i | m | e | : | | H | H | : | M | M |
| Row 2 | | U | p | + | | o | r | | D | o | w | n | - | | | |

As displayed:
```
 Set Time: HH:MM
 Up+ or Down-   
```

## SetTime-Confirm

Same Row 1 and same valid range as `SetTime-Adjust`. Row 2 has a
leading space and a trailing space, matching `SetTemp-Decision`'s
"Enter Y Mode N" layout — **not** the no-leading-space variant used by
`SetTemp-Confirm`. Preserved exactly as specified per screen.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | S | e | t | | T | i | m | e | : | | H | H | : | M | M |
| Row 2 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |

As displayed:
```
 Set Time: HH:MM
 Enter Y Mode N 
```