# Idle / Off Mode

Content owner for `Idle-Splash` and `Idle-Prompt` (see `states_modes.md`
for transitions, timeouts, and the Settings-entry combo — this file
covers screen content/wording only).

Character positions below are taken directly from the source workbook's
raw cell data (V1 tab), read programmatically to avoid transcription
error. Position 1 is the leftmost column of the 16-column LCD.

Note: the workbook cell at Idle-Prompt Row 1, position 9 contained a
typo (`'t '` with a trailing space) — corrected to `'t'`, one LCD
character per column.

## Idle-Splash

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | | | | | T | a | y | l | o | r | | | | | |
| Row 2 | | P | r | o | o | f | i | n | g | | O | v | e | n | | |

As displayed:
```
     Taylor     
 Proofing Oven  
```

## Idle-Prompt

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | | T | o | | S | t | a | r | t | |  |  |  |  |  | |
| Row 2 | | P | r | e | s | s | | M| o | d | e | | | | | |

As displayed:
```
 To Start Press 
 Mode           
```