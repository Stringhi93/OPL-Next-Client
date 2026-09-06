#include "input.h"

#include <stdio.h>
#include <string.h>

#include <kernel.h>
#include <sifrpc.h>
#include <libpad.h>

#define PAD_PORT        0
#define PAD_SLOT        0

static int initialized = 0;
static unsigned int old_buttons = 0;
static unsigned int current_buttons = 0;

static char padBuf[256] __attribute__((aligned(64)));

static int pad_read(void)
{
    struct padButtonStatus buttons;

    int state;

    state = padGetState(PAD_PORT, PAD_SLOT);

    if (state != PAD_STATE_STABLE &&
        state != PAD_STATE_FINDCTP1)
    {
        return 0;
    }

    if (padRead(PAD_PORT, PAD_SLOT, &buttons) <= 0)
    {
        return 0;
    }

    current_buttons =
        ((unsigned int)buttons.btns[0] << 8) |
        (unsigned int)buttons.btns[1];

    return 1;
}

void input_init(void)
{
    if (initialized)
        return;

    SifInitRpc(0);

    memset(padBuf, 0, sizeof(padBuf));

    padInit(0);

    if (padPortOpen(PAD_PORT, PAD_SLOT, padBuf) == 0)
    {
        scr_printf("INPUT: erro ao abrir controle\n");
        initialized = 0;
        return;
    }

    initialized = 1;

    old_buttons = 0;
    current_buttons = 0;

    scr_printf("INPUT: OK\n");
}

void input_update(void)
{
    if (!initialized)
        return;

    old_buttons = current_buttons;

    pad_read();
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
     * libpad usa bits ativos em 0.
     * Detectamos aqui a transicao:
     * botao nao pressionado -> pressionado.
     */
    if ((current_buttons & mask) == 0 &&
        (old_buttons & mask) != 0)
    {
        return 1;
    }

    /*
     * Na primeira leitura, permite detectar
     * um botao que ja esteja pressionado.
     */
    if (old_buttons == 0 &&
        (current_buttons & mask) == 0)
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
    old_buttons = 0;
    current_buttons = 0;
}
