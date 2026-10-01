# Error Mode

---
| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | E | r | r | : |  | X | X |  |  |  |  | | | | ||
| Row 2 | T | u | r | n |  | P | r | o | o | f | e | r |  | O | f | f |

As displayed:
```
 Err: XX  
 Turn Proofer Off
```
Toggle row 2 every 2 seconds. 

| Pos | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Row 1 | E | r | r | : |  | X | X |  |  |  |  | | | | ||
| Row 2 | C | o | n | t | a | c | t |  | S | u | p | p | o | r | t |  |

As displayed:
```
 Err: XX  
 Contact Support
```
---
## Error Action
When an error is detected, turn off actuators and display these screens. There is no time out.
Ignore any user button input and any communication input.
The system shall have a timed thermal fuse, that will open if the heater fails on.
For example, if the heater fails on, and the user is unaware, the fuse shall open after 12 hours of continuous heating.
Note: the system shall never use firmware as part of the safety system!

---
## Error Codes
See error_codes.md for error codes.

---