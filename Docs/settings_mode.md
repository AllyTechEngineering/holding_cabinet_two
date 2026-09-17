# Settings Mode

Content owner for `Settings-Splash`, `Settings-Adjust`, `Settings-Confirm`
— see `states_modes.md` for transitions/timeouts (the Up+Down-held-5s
entry combo, Idle-only) and the resolved decision to build this as
generic scrollable-menu infrastructure. This file covers screen
content/wording only, and currently defines the one setting that
exists today (Temp units, F/C).


## Settings-Splash

Shown once for 2 seconds, then auto-advances to `Settings-Adjust` (not
a toggle pair — see `states_modes.md`'s "Auto transitions" section).
Row 2 shows the current live value of the first (and currently only)
setting.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | | S | e | t | t | i | n | g | s | | M | e | n | u | |
| Row 2 | | | | | | T | e | m | p | : | F | | | | | |

As displayed:
```
  Settings Menu 
     Temp:F     
```

## Settings-Adjust

Row 2 shows the current live value; Up/Down toggles it between F and C.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | U | p | | F | | o | r | | D | o | w | n | | C | |
| Row 2 | | | | | | T | e | m | p | : | F | | | | | |

As displayed:
```
 Up F or Down C 
     Temp:F     
```

## Settings-Confirm

Row 2 shows the current live value (same field as `Settings-Adjust`).
Enter (from either `Settings-Adjust` or `Settings-Confirm`) saves and
returns to `Idle-Splash`.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |
| Row 2 | | | | | | T | e | m | p | : | F | | | | | |

As displayed:
```
 Enter Y Mode N 
     Temp:F     
```

**Examples from source** — default is F; the Row 2 value reflects
whichever unit is currently selected:

**Example 1 (default)**

| | |
|---|---|
| Row 1 | `Enter Y Mode N` |
| Row 2 | `Temp:F` |

**Example 2 (C selected)**

| | |
|---|---|
| Row 1 | `Enter Y Mode N` |
| Row 2 | `Temp:C` |