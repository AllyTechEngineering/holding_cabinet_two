# Set Temp Mode

Content owner for `SetTemp-Decision`, `SetTemp-Adjust`, `SetTemp-Confirm`
(see `states_modes.md` for transitions/timeouts/heat-lifecycle rules —
this file covers screen content/wording and this screen's value bounds).

Character positions taken directly from the source workbook's raw cell
data (V1 tab), read programmatically.

## SetTemp-Decision

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | T | o | | S | e | t | | T | e | m | p | | | | |
| Row 2 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |

As displayed:
```
 To Set Temp    
 Enter Y Mode N 
```

## SetTemp-Adjust

`XXX` is a 3-digit placeholder for the live setpoint value (no
leading-zero padding, e.g. `95` not `095`); `F` reflects the current
Settings unit choice. Valid range: **65–120°F** (clamped; Up/Down has
no effect past either bound).

**Physical constraint:** the system has no active cooling, only a heat
relay. A setpoint below the current ambient/cabinet temperature is a
valid input but will never actually be reached — the heater simply
stays off and the cabinet drifts to ambient. Not a fault condition, no
special handling planned; noted here so it isn't mistaken for a bug
during bring-up. **Carry this forward into `HeatTask`'s control-loop
design** — there's no dedicated control-loop doc yet.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | S | e | t | | T | e | m | p | : |   | X | X | X | F | |
| Row 2 | | U | p | + | | o | r | | D | o | w | n | - | | | |

As displayed:
```
 Set Temp: XXXF  
 Up+ or Down-   
```

## SetTemp-Confirm

Same Row 1 and same valid range as `SetTemp-Adjust`. Row 2 is laid out
differently from `SetTemp-Decision`'s "Enter Y Mode N" — no leading
space, two trailing spaces — preserved exactly as specified.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | S | e | t | | T | e | m | p | : |   | X | X | X | F | |
| Row 2 | E | n | t | e | r | | Y | | M | o | d | e | | N | | |

As displayed:
```
 Set Temp: XXXF  
Enter Y Mode N  
```