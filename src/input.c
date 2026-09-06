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
    int ret;
    int state;
    int timeout;

    if (initialized)
        return;

    scr_printf("INPUT: inicializando...\n");

    /*
     * Inicializa o sistema de controle.
     */
    ret = padInit(0);

    if (ret < 0)
    {
        scr_printf("INPUT: padInit ERRO\n");
        return;
    }

    memset(
        padBuf,
        0,
        sizeof(padBuf)
    );

    /*
     * Abre porta 0 / slot 0.
     */
    ret = padPortOpen(
        PAD_PORT,
        PAD_SLOT,
        padBuf
    );

    if (ret == 0)
    {
        scr_printf("INPUT: controle nao encontrado\n");
        return;
    }

    scr_printf("INPUT: aguardando controle...\n");

    /*
     * Aguarda o controle ser detectado.
     *
     * Nao usamos DelayThread(), pois algumas versoes
     * do PS2SDK nao expoem essa funcao.
     */
    timeout = 0;

    while (1)
    {
        state = padGetState(
            PAD_PORT,
            PAD_SLOT
        );

        if (state == PAD_STATE_STABLE)
            break;

        if (state == PAD_STATE_DISCONN)
        {
            scr_printf("INPUT: controle desconectado\n");

            padPortClose(
                PAD_PORT,
                PAD_SLOT
            );

            padEnd();

            return;
        }

        /*
         * Evita ficar preso para sempre se nao houver
         * um controle funcional.
         */
        timeout++;

        if (timeout > 5000000)
        {
            scr_printf("INPUT: timeout aguardando controle\n");

            padPortClose(
                PAD_PORT,
                PAD_SLOT
            );

            padEnd();

            return;
        }
    }

    initialized = 1;

    old_buttons = 0xFFFF;
    current_buttons = 0xFFFF;

    scr_printf("INPUT: CONTROLE OK\n");
}

void input_update(void)
{
    struct padButtonStatus status;
    int state;

    if (!initialized)
        return;

    /*
     * Verifica o estado do controle.
     */
    state = padGetState(
        PAD_PORT,
        PAD_SLOT
    );

    if (state != PAD_STATE_STABLE)
        return;

    /*
     * Guarda o estado anterior.
     */
    old_buttons = current_buttons;

    /*
     * Le os botoes.
     */
    if (padRead(
            PAD_PORT,
            PAD_SLOT,
            &status
        ) <= 0)
    {
        return;
    }

    /*
     * btns e um valor de 16 bits.
     *
     * 0 = pressionado
     * 1 = solto
     */
    current_buttons = (unsigned int)status.btns;
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
     * Detecta somente o momento em que o botao
     * passa de solto para pressionado.
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

    padPortClose(
        PAD_PORT,
        PAD_SLOT
    );

    padEnd();

    initialized = 0;

    old_buttons = 0xFFFF;
    current_buttons = 0xFFFF;

    scr_printf("INPUT: desligado\n");
}
