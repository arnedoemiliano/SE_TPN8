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
//#define RES_RELOJ         6    // Cuantos digitos tiene el reloj
#define RES_DISPLAY_RELOJ 4    // Cuantos digitos del reloj se mostrarán
#define INT_PER_SECOND    1000 // interrupciones por segundo del systick
/* === Private data type declarations ========================================================== */

typedef enum {
    SIN_CONFIGURAR,
    MOSTRANDO_HORA,
    AJUSTANDO_MINUTOS_ACTUAL,
    AJUSTANDO_HORAS_ACTUAL,
    AJUSTANDO_MINUTOS_ALARMA,
    AJUSTANDO_HORAS_ALARMA,
} modo_t;

/* === Private variable declarations =========================================================== */

static board_t board;
static reloj_t reloj;
static uint8_t temp_input[4]; // 4 porque nunca se configura la hora por minutos
static const uint8_t limite_min[] = {5, 9};
static const uint8_t limite_hs[] = {2, 3};

/* === Private function declarations ===========================================================
 */

void ActivarAlarma(reloj_t reloj, bool act_desact);
void CambiarModo(modo_t modo);
void IncrementarBCD(uint8_t numero[2], const uint8_t limite[2]);
// limite indica donde se pasa despues de restar 1 a 00
void DecrementarBCD(uint8_t numero[2], const uint8_t limite[2]);

/* === Public variable definitions ============================================================= */
modo_t modo;
/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

void ActivarAlarma(reloj_t reloj, bool act_desact) {
}

void CambiarModo(modo_t valor) {
    modo = valor;

    switch (modo) {
    case SIN_CONFIGURAR:
        DisplayFlashDigits(board->display, 0, 3, 50);
        break;
    case MOSTRANDO_HORA:
        DisplayFlashDigits(board->display, 0, 3, 0); // digitos sin parpadear
        // SetClockTime(reloj, (uint8_t[]){1, 2, 3, 4}, RES_DISPLAY_RELOJ);
        break;
    case AJUSTANDO_MINUTOS_ACTUAL:
        DisplayFlashDigits(board->display, 2, 3, 50);
        break;
    case AJUSTANDO_HORAS_ACTUAL:
        DisplayFlashDigits(board->display, 0, 1, 50);
        break;
    case AJUSTANDO_MINUTOS_ALARMA:
        DisplayFlashDigits(board->display, 2, 3, 50);
        break;
    case AJUSTANDO_HORAS_ALARMA:
        DisplayFlashDigits(board->display, 0, 1, 50);
        break;
    default:
        break;
    }
}

void IncrementarBCD(uint8_t numero[2], const uint8_t limite[2]) {

    // uint8_t temp_limite_1 = limite[1];

    numero[1]++;

    // Corroboro antes si se llego al limite para que el ahora segundo if no me rompa la condicion
    // de limite
    if ((numero[0] == limite[0]) && (numero[1] > limite[1])) {
        numero[0] = 0;
        numero[1] = 0;
    }

    if (numero[1] > 9) {

        numero[1] = 0;
        numero[0]++;
    }
}

void DecrementarBCD(uint8_t numero[2], const uint8_t limite[2]) {
    numero[1]--;

    if ((numero[0] == 0) && ((int8_t)numero[1] < 0)) {
        numero[0] = limite[0];
        numero[1] = limite[1];
    }
    if ((int8_t)numero[1] < 0) {

        numero[1] = 9;
        numero[0]--;
    }
}

/* === Public function implementation ========================================================= */

int main(void) {

    reloj = ClockCreate(TICKS_PER_SECOND, ActivarAlarma);
    board = BoardCreate();
    modo = SIN_CONFIGURAR;
    SisTick_Init(INT_PER_SECOND);
    DisplayFlashDigits(board->display, 0, 3, 50); // cuando inicia el reloj los digitos parpadean

    while (1) {
        /*

        Problemas: las funciones memset y todas las que modifican reloj->memory, ponen en 0 el bit
        del punto. La que mas lo hace es DisplayWriteBCD que se la llama cada segundo. Tampoco puedo
        llamarla cada minuto porque sino nunca podría mostrar los segundos. La solucion mas viable,
        es cambiar el memset y resetear los 7 LSB de cada elemento de memory manualmente.


        */
        // ACEPTAR
        if (DigitalInputHasActivated(board->accept)) {
            if (modo == SIN_CONFIGURAR) { // despues de 3 segundos
                CambiarModo(MOSTRANDO_HORA);
            } else if (modo == AJUSTANDO_MINUTOS_ACTUAL) {
                CambiarModo(AJUSTANDO_HORAS_ACTUAL);
            } else if (modo == AJUSTANDO_MINUTOS_ALARMA) {
                CambiarModo(AJUSTANDO_HORAS_ALARMA);
            } else if (modo == AJUSTANDO_HORAS_ACTUAL) {
                CambiarModo(MOSTRANDO_HORA);
                SetClockTime(reloj, temp_input, sizeof(temp_input));
            } else if (modo == AJUSTANDO_HORAS_ALARMA) {
                CambiarModo(MOSTRANDO_HORA);
                SetAlarmTime(reloj, temp_input);
            }
        }
        // CANCELAR
        if (DigitalInputHasActivated(board->cancel)) {
            if (modo == AJUSTANDO_MINUTOS_ACTUAL || modo == AJUSTANDO_HORAS_ACTUAL) {
                CambiarModo(SIN_CONFIGURAR);
                // temp_input = {0, 0, 0, 0};
            }
        }
        // F1
        if (DigitalInputHasActivated(board->set_time)) {
            CambiarModo(AJUSTANDO_MINUTOS_ACTUAL);
            GetClockTime(reloj, temp_input, sizeof(temp_input));
            DisplayWriteBCD(board->display, temp_input, sizeof(temp_input));
        }
        // F2
        if (DigitalInputHasActivated(board->set_alarm)) {
            CambiarModo(AJUSTANDO_MINUTOS_ALARMA);
            GetAlarmTime(reloj, temp_input);
            DisplayWriteBCD(board->display, temp_input, sizeof(temp_input));
        }

        // F3
        if (DigitalInputHasActivated(board->decrement)) {
            if (modo == AJUSTANDO_MINUTOS_ACTUAL || modo == AJUSTANDO_MINUTOS_ALARMA) {
                DecrementarBCD(&temp_input[2], limite_min);
            } else if (modo == AJUSTANDO_HORAS_ACTUAL || modo == AJUSTANDO_HORAS_ALARMA) {
                DecrementarBCD(temp_input, limite_hs);
            }
            DisplayWriteBCD(board->display, temp_input, sizeof(temp_input));
        }

        // F4
        if (DigitalInputHasActivated(board->increment)) {
            if (modo == AJUSTANDO_MINUTOS_ACTUAL || modo == AJUSTANDO_MINUTOS_ALARMA) {
                // le paso el puntero a los dos digitos menos significativos
                IncrementarBCD(&temp_input[2], limite_min);
            } else if (modo == AJUSTANDO_HORAS_ACTUAL || modo == AJUSTANDO_HORAS_ALARMA) {
                IncrementarBCD(temp_input, limite_hs);
            }
            DisplayWriteBCD(board->display, temp_input, sizeof(temp_input));
        }

        for (int index = 0; index < 100; index++) {
            for (int delay = 0; delay < 25000; delay++) {
                __asm("NOP");
            }
        }
    }
}

void SysTick_Handler(void) {

    uint8_t hora[RES_DISPLAY_RELOJ];
    int tick = RelojNuevoTick(reloj);
    if (modo <= MOSTRANDO_HORA) {

        if (tick == TICKS_PER_SECOND - 1 || tick == (TICKS_PER_SECOND) / 2) {
            (void)GetClockTime(reloj, hora, RES_DISPLAY_RELOJ);
            DisplayToggleDot(board->display, 1);
            DisplayWriteBCD(board->display, hora, RES_DISPLAY_RELOJ);
        }
    }

    DisplayRefresh(board->display);
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */

/*
if (numero[0] < limite[0]) {
            numero[1] = 0;
        }

*/
