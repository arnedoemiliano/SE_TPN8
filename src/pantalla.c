/************************************************************************************************
Copyright (c) 2023, Emiliano Arnedo <emiarnedo@gmail.com>
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
SPDX-License-Identifier: MIT
*************************************************************************************************/

/** \brief Brief description of the file
 **
 ** Full file description
 **
 ** \addtogroup name Module denomination
 ** \brief Brief description of the module
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "pantalla.h"
#include "string.h"

/* === Macros definitions ====================================================================== */

#if !defined(DISPLAY_MAX_DIGITS)
    #define DISPLAY_MAX_DIGITS 8
#endif

/* === Private data type declarations ========================================================== */

struct display_s {
    uint8_t digits;                     // cantidad de digitos del display
    uint8_t active_digit;               // digito activo
    uint8_t memory[DISPLAY_MAX_DIGITS]; //
    struct display_driver_s driver[1];
};

/* === Private variable declarations =========================================================== */

static const uint8_t IMAGES[] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,
    SEGMENT_B | SEGMENT_C,
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,
    SEGMENT_A | SEGMENT_B | SEGMENT_C,
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,
};

/* === Private function declarations =========================================================== */

display_t DisplayAllocate();

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */
display_t DisplayAllocate() {

    static struct display_s instances[1] = {0};
    return &instances[0];
}
/* === Public function implementation ========================================================== */

display_t DisplayCreate(uint8_t digits, display_driver_t driver) {

    display_t display = DisplayAllocate();
    display->digits = digits;           // cantidad de digitos que tiene el display (4)
    display->active_digit = digits - 1; // comienza el ultimo digito como activo (3)
    memcpy(display->driver, driver, sizeof(display->driver));
    memset(display->memory, 0, sizeof(display->memory)); // limpia la memoria
    display->driver->ScreenTurnOff();                    // apaga todos los digitos

    return display;
}

void DisplayWriteBCD(display_t display, uint8_t * numbers, uint8_t size) {

    memset(display->memory, 0, sizeof(display->memory));

    for (int i = 0; i < size; i++) {

        if (i >= display->digits)
            break;
        display->memory[i] = IMAGES[numbers[i]];
    }
}

void DisplayRefresh(display_t display) {
    display->driver->ScreenTurnOff;
    // incrementa active_digit y se reinicia cuando llega al valor maximo
    display->active_digit = (display->active_digit + 1) % display->digits;
    display->driver->SegmentsTurnOn(display->memory[display->active_digit]);
    display->driver->DigitTurnOn(display->active_digit);

    /*
        display->digits--;
        if (display->active_digit == 0) {
            display->active_digit = display->digits - 1;
        }
    */
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */