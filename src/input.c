#include "input.h"

#include <stdio.h>
#include <string.h>

#include <kernel.h>
#include <sifrpc.h>
#include <debug.h>
#include <libpad.h>

#define PAD_PORT 0
#define PAD_SLOT 0

static int initialized = 0;

static unsigned int old_buttons = 0xFFFF;
static unsigned int current_buttons = 0xFFFF;

static char padBuf[256] __attribute__((aligned(64)));

void input_init(void)
{
    if (initialized)
        return;

    /*
     * Inicializa o sistema de controle.
     */
    padInit(0);

    memset(padBuf, 0, sizeof(padBuf));

    /*
     * Abre o controle na porta 0, slot 0.
     */
    if (padPortOpen(PAD_PORT, PAD_SLOT, padBuf) == 0)
    {
        scr_printf("INPUT: ERRO AO ABRIR CONTROLE\n");
        return;
    }

    initialized = 1;

    old_buttons = 0xFFFF;
    current_buttons = 0xFFFF;

    scr_printf("INPUT: OK\n");
}

void input_update(void)
{
    struct padButtonStatus status;

    if (!initialized)
        return;

    /*
     * Guarda o estado anterior.
     */
    old_buttons = current_buttons;

    /*
     * Lê o estado atual do controle.
     */
    if (padRead(PAD_PORT, PAD_SLOT, &status) <= 0)
        return;

    /*
     * No PS2SDK usado pelo projeto, btns é um
     * valor de 16 bits, e não um array.
     */
    current_buttons = status.btns;
}

int input_pressed(int button)
{
    unsigned int mask;

    if (!initialized)
        return 0;

    switch (button)
    {
        case INPUT_UP:
            mask = PAD_UP;
            break;

        case INPUT_DOWN:
            mask = PAD_DOWN;
            break;

        case INPUT_LEFT:
            mask = PAD_LEFT;
            break;

        case INPUT_RIGHT:
            mask = PAD_RIGHT;
            break;

        case INPUT_CROSS:
            mask = PAD_CROSS;
            break;

        case INPUT_CIRCLE:
            mask = PAD_CIRCLE;
            break;

        case INPUT_START:
            mask = PAD_START;
            break;

        case INPUT_SELECT:
            mask = PAD_SELECT;
            break;

        default:
            return 0;
    }

    /*
     * Os botoes do DualShock usam logica ativa em 0:
     *
     * 0 = pressionado
     * 1 = solto
     *
     * Detecta somente a transicao:
     *
     * anterior = solto
     * atual    = pressionado
     */
    if ((current_buttons & mask) == 0 &&
        (old_buttons & mask) != 0)
    {
        return 1;
    }

    return 0;
}

void input_shutdown(void)
{
    if (!initialized)
        return;

    padPortClose(PAD_PORT, PAD_SLOT);
    padEnd();

    initialized = 0;

    old_buttons = 0xFFFF;
    current_buttons = 0xFFFF;
}
