# Coding Standards

## 1. Purpose

This document defines source-code formatting and documentation conventions for
AllyTech-maintained firmware source files.


## 2. File Header

AllyTech-maintained `.c` and `.h` files shall begin with the standard AllyTech
file header.

Source file:

    /********************************************************************************
     * @file           : example.c
     * @brief          : Example implementation
     ******************************************************************************
     * @attention
     *
     * Copyright (c) 2026 AllyTech LLC.
     * All rights reserved.
     ********************************************************************************/

Header file:

    /********************************************************************************
     * @file           : example.h
     * @brief          : Header for example.c
     ******************************************************************************
     * @attention
     *
     * Copyright (c) 2026 AllyTech LLC.
     * All rights reserved.
     ********************************************************************************/


## 3. Function Documentation

Public functions and application-defined callbacks shall have Doxygen-style
function documentation.

Documentation shall describe the function contract rather than restating the
implementation.

Document, as applicable:

- Purpose and behavior.
- Parameters using `@param`.
- Return value using `@return`.
- Important side effects.
- ISR, RTOS, concurrency, timing, or hardware constraints.

Example:

    /**
     * @brief Initializes the UART transport layer.
     *
     * Associates the transport with the UART peripheral and receive queue.
     *
     * @param huart UART handle used by the transport.
     * @param rx_queue Queue that receives incoming UART bytes.
     *
     * @return HAL_OK if initialization succeeds; HAL_ERROR otherwise.
     */


## 4. Private Function Documentation

Static functions do not require full Doxygen documentation when their purpose
and behavior are clear.

Add documentation when behavior, assumptions, side effects, hardware
dependencies, timing, or concurrency constraints are not obvious.


## 5. Inline Comments

Inline comments shall be used only when information necessary to understand
the implementation is not readily apparent from the code.

Inline comments shall not restate requirements, architecture, HMI behavior,
hardware documentation, or other information already defined in authoritative
project documentation.

Comments may explain implementation-specific information such as:

- Why a non-obvious coding technique is necessary.
- Library, HAL, compiler, or language behavior relevant to the implementation.
- A workaround that would otherwise appear unnecessary.
- An implementation assumption that cannot be expressed clearly by the code.

Do not add comments that merely restate the code.

Avoid:

    /* Increment error count */
    error_count++;


## 6. Comment Maintenance

Comments shall be updated when the associated implementation changes.

Incorrect or obsolete comments shall be corrected or removed.