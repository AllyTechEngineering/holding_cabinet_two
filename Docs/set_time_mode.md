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

`HH:MM` shows the proposed countdown duration without a seconds field or
a leading zero on the hour (`1:00`, `4:30`, `10:00`). For a new proof,
adjustment starts at 1:00. The valid range is 0:15 through 10:00.
Up/Down stops at either bound; the value does not wrap around.

A debounced Up or Down press changes the duration by one minute
immediately. If the button remains held, changes repeat as follows:

| Time since press | Change |
|---|---|
| Less than 0.6 seconds | No repeat |
| 0.6 to less than 2 seconds | 1 minute every 200 ms |
| 2 to less than 4 seconds | 5 minutes every 400 ms |
| 4 seconds or longer | 5 minutes every 200 ms |

Repeat timing is measured from the previous repeat, so crossing a hold
threshold does not itself cause an extra change. Releasing the button
stops repetition. Pressing the opposite direction starts a new hold
ramp. The same controls work on `SetTime-Adjust` and `SetTime-Confirm`.
Their 2-second screen alternation continues during adjustment; Up/Down
does not restart it.

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

## Editing an active run

The existing countdown continues while the time screens are displayed.
At `SetTime-Decision`, Enter opens the existing Adjust/Confirm screens.
Mode skips the time edit and opens `Run-Decision`.

Up/Down changes a proposed duration on either `SetTime-Adjust` or
`SetTime-Confirm`. Enter retains that proposal and opens
`Run-Decision`; it does not yet change the active countdown. Mode
discards the proposed duration and returns to `SetTime-Decision`.

If the user later presses Enter at `Run-Decision`, the proposed
duration starts a new countdown from that moment. All time elapsed
before that confirmation is ignored. If the user did not change the
time, the existing countdown continues without restarting.