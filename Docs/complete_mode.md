# Complete Mode

Content owner for `Complete-Decision`, `Complete-DisplayA`,
`Complete-DisplayB` — see `states_modes.md` for transitions/timeouts/
heat-lifecycle rules; this file covers screen content/wording only.


## Complete-Decision

Reached when the user presses Enter during `Run-Active`, or when a
timed run's countdown reaches 0:00. Mode during `Run-Active` opens
the run-edit path instead. Pressing Enter at `Complete-Decision`
confirms completion and stops heat immediately.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | P | r | o | o | f | | C | o | m | p | l | e | t | e | ? |
| Row 2 | | E | n | t | e | r | | Y | | M | o | d | e | | N | |

As displayed:
```
 Proof Complete?
 Enter Y Mode N 
```

## Complete-DisplayA

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | P | r | o | o | f | | C | o | m | p | l | e | t | e | |
| Row 2 | | P | r | e | s | s | | M | o | d | e | | t | o | | |

As displayed:
```
 Proof Complete 
 Press Mode to  
```

## Complete-DisplayB

Row 1 identical to `Complete-DisplayA`.

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | P | r | o | o | f | | C | o | m | p | l | e | t | e | |
| Row 2 | | S | t | a | r | t | | A | g | a | i | n | | | | |

As displayed:
```
 Proof Complete 
 Start Again    
```