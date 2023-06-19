/* Copyright 2022, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2022, Esteban Volentini <evolentini@herrera.unt.edu.ar>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Simple sample of use LPC HAL gpio functions
 **
 ** \addtogroup samples Sample projects
 ** \brief Sample projects to use as a starting point
 ** @{ */

/* === Headers files inclusions =============================================================== */
#include "bsp.h"
#include "reloj.h"
#include "chip.h"
#include <stdbool.h>

/* === Macros definitions ====================================================================== */
#define RES_RELOJ         6    // Cuantos digitos tiene el reloj
#define RES_DISPLAY_RELOJ 4    // Cuantos digitos del reloj se mostrarán
#define INT_PER_SECOND    1000 // interrupciones por segundo del systick
/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

static board_t board;
static reloj_t reloj;

/* === Private function declarations =========================================================== */

void ActivarAlarma(reloj_t reloj, bool act_desact);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

static uint8_t hora_actual[4] = {1, 7, 5, 6};

/* === Private function implementation ========================================================= */

void ActivarAlarma(reloj_t reloj, bool act_desact) {
}

/* === Public function implementation ========================================================= */

int main(void) {

    SisTick_Init(INT_PER_SECOND);
    reloj = ClockCreate(TICKS_PER_SECOND, ActivarAlarma);
    board = BoardCreate();

    DisplayWriteBCD(board->display, hora_actual, RES_DISPLAY_RELOJ);
    // DisplayFlashDigits(board->display, 0, 3, 0);

    // SetClockTime(reloj, hora_actual, RES_RELOJ);

    while (1) {

        if (DigitalInputHasActivated(board->accept)) {
            // DisplayWriteBCD(board->display, (uint8_t[]){1, 1, 1, 1}, 4);
        }

        if (DigitalInputHasActivated(board->cancel)) {
        }

        if (DigitalInputHasActivated(board->set_time)) {
        }

        if (DigitalInputHasActivated(board->set_alarm)) {
        }

        if (DigitalInputHasActivated(board->decrement)) {
        }

        if (DigitalInputHasActivated(board->increment)) {
        }

        for (int index = 0; index < 100; index++) {
            for (int delay = 0; delay < 25000; delay++) {
                __asm("NOP");
            }
        }
    }
}

void SysTick_Handler(void) {

    int tick = RelojNuevoTick(reloj);
    if (tick == TICKS_PER_SECOND - 1 || tick == (TICKS_PER_SECOND) / 2) {
        DisplayToggleDot(board->display, 1);
    }
    DisplayRefresh(board->display);
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
