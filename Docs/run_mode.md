# Run Mode

Content owner for `Run-Decision` and `Run-Active` (timed and untimed
variants) — see `states_modes.md` for transitions/timeouts/heat-lifecycle
rules; this file covers screen content/wording only.


## Run-Decision

Heat is **off** at this screen — it only turns on once Enter is pressed
and `Run-Active` is entered (see `states_modes.md`).

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | T | o | | S | t | a | r | t | | P | r | o | o | f | |
| Row 2 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |

As displayed:
```
 To Start Proof 
 Enter Y Mode N 
```

## Run-Active (timed variant)

`XXX` = live NTC reading, no leading-zero padding (`98`, not `098`).
`HH:MM` = live countdown, no leading zeros on the hour digit(s)
(`4:30` for one hour, not `04:30`). Update rate: ~1 second, readable,
no jitter (per source note — not a hard requirement, just the intent).

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | T | e | m | p | : | | X | X | X | F | | | | | |
| Row 2 | | T | i | m | e | : | | H | H | : | M | M | | | | |

As displayed:
```
 Temp: XXXF     
 Time: HH:MM    
```

Field width is **not fixed** — no leading-zero padding means `Temp:`
and `Time:` change width with the value. `DisplayTask` must actively
clear trailing characters when a value shrinks (e.g. `100F` → `98F`
leaves a stale `0` unless cleared; `10:00` → `4:30` likewise).

**Example 1**

| | |
|---|---|
| Row 1 | `Temp: 98F` |
| Row 2 | `Time: 4:30` |

**Example 2**

| | |
|---|---|
| Row 1 | `Temp: 100F` |
| Row 2 | `Time: 10:00` |

## Run-Active (untimed variant)

Row 1 is identical in format to the timed variant. Row 2 toggles every
2 seconds between "Countdown Timer" and "Not Used" — forever, until the
user stops the run (per `states_modes.md`'s toggle-pair rule; this is
the one case where a toggle appears *within* `Run-Active` itself rather
than between two separate states).

**Example 3**

| | |
|---|---|
| Row 1 | `Temp: 98F` |
| Row 2 | `Countdown Timer` |

**Example 4**

| | |
|---|---|
| Row 1 | `Temp: 100F` |
| Row 2 | `Not Used` |