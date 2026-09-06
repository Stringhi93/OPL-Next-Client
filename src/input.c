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

/*
 * Inicializa o controle.
 */
void input_init(void)
{
    int ret;

    if (initialized)
        return;

    scr_printf("INPUT: inicializando...\n");

    /*
     * Inicializa o sistema do pad.
     */
    ret = padInit(0);

    if (ret < 0)
    {
        scr_printf("INPUT: padInit ERRO\n");
        return;
    }

    /*
     * Limpa o buffer.
     */
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

    /*
     * Aguarda o controle ficar estavel.
     */
    scr_printf("INPUT: aguardando controle...\n");

    while (1)
    {
        int state;

        state = padGetState(
            PAD_PORT,
            PAD_SLOT
        );

        if (state == PAD_STATE_STABLE)
            break;

        if (state == PAD_STATE_FINDCTP1)
        {
            /*
             * O controle ainda esta sendo detectado.
             */
            DelayThread(1000);
            continue;
        }

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

        DelayThread(1000);
    }

    initialized = 1;

    old_buttons = 0xFFFF;
    current_buttons = 0xFFFF;

    scr_printf("INPUT: CONTROLE OK\n");
}

/*
 * Atualiza o estado do controle.
 */
void input_update(void)
{
    struct padButtonStatus status;
    int state;

    if (!initialized)
        return;

    /*
     * Verifica se o controle continua conectado.
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
     * Le o controle.
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
     * btns contem os botoes em 16 bits.
     *
     * Bit = 0 -> pressionado
     * Bit = 1 -> solto
     */
    current_buttons = (unsigned int)status.btns;
}

/*
 * Retorna 1 somente quando o botao acabou de ser pressionado.
 */
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
     * Detecta a transicao:
     *
     * antigo = solto
     * atual  = pressionado
     */
    if ((current_buttons & mask) == 0 &&
        (old_buttons & mask) != 0)
    {
        return 1;
    }

    return 0;
}

/*
 * Finaliza o controle.
 */
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
