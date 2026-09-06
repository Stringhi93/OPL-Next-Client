#include "input.h"

#include <stdio.h>
#include <string.h>

#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <debug.h>
#include <libpad.h>

#define PAD_PORT 0
#define PAD_SLOT 0

/*
 * Esses dois IRX sao gerados pelo Makefile
 * e ficam embutidos no ELF.
 */
extern unsigned char SIO2MAN_irx[];
extern unsigned int size_SIO2MAN_irx;

extern unsigned char PADMAN_irx[];
extern unsigned int size_PADMAN_irx;

static int initialized = 0;

static unsigned short old_buttons = 0xFFFF;
static unsigned short current_buttons = 0xFFFF;

static unsigned char padBuf[256] __attribute__((aligned(64)));

static int load_pad_modules(void)
{
    int ret;

    scr_printf("PAD: carregando SIO2MAN...\n");

    ret = SifExecModuleBuffer(
        SIO2MAN_irx,
        size_SIO2MAN_irx,
        0,
        NULL,
        NULL
    );

    if (ret < 0)
    {
        scr_printf("PAD: SIO2MAN ERRO %d\n", ret);
        return 0;
    }

    scr_printf("PAD: carregando PADMAN...\n");

    ret = SifExecModuleBuffer(
        PADMAN_irx,
        size_PADMAN_irx,
        0,
        NULL,
        NULL
    );

    if (ret < 0)
    {
        scr_printf("PAD: PADMAN ERRO %d\n", ret);
        return 0;
    }

    return 1;
}

void input_init(void)
{
    int ret;
    int state;
    int timeout;

    if (initialized)
        return;

    scr_printf("INPUT: inicializando...\n");

    /*
     * Os drivers do controle sao IOP modules.
     */
    if (!load_pad_modules())
        return;

    /*
     * Inicializa libpad.
     */
    ret = padInit(0);

    if (ret <= 0)
    {
        scr_printf("INPUT: padInit ERRO %d\n", ret);
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

    if (ret <= 0)
    {
        scr_printf("INPUT: padPortOpen ERRO\n");
        padEnd();
        return;
    }

    scr_printf("INPUT: procurando controle...\n");

    /*
     * Aguarda o PADMAN detectar o controle.
     *
     * O estado STABLE = 6.
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
            if ((timeout % 120000) == 0)
                scr_printf("INPUT: sem controle...\n");
        }

        timeout++;

        /*
         * Evita travar eternamente a inicializacao.
         */
        if (timeout > 3000000)
        {
            scr_printf("INPUT: timeout\n");

            padPortClose(
                PAD_PORT,
                PAD_SLOT
            );

            padEnd();

            return;
        }

        nopdelay();
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
    int result;

    if (!initialized)
        return;

    state = padGetState(
        PAD_PORT,
        PAD_SLOT
    );

    if (state == PAD_STATE_DISCONN)
    {
        initialized = 0;
        return;
    }

    /*
     * O pad precisa estar STABLE para leitura.
     */
    if (state != PAD_STATE_STABLE)
        return;

    old_buttons = current_buttons;

    result = padRead(
        PAD_PORT,
        PAD_SLOT,
        &status
    );

    if (result <= 0)
        return;

    /*
     * libpad define btns como unsigned short.
     *
     * 0 = pressionado
     * 1 = solto
     */
    current_buttons = status.btns;
}

static unsigned short get_mask(int button)
{
    switch (button)
    {
        case INPUT_UP:
            return PAD_UP;

        case INPUT_DOWN:
            return PAD_DOWN;

        case INPUT_LEFT:
            return PAD_LEFT;

        case INPUT_RIGHT:
            return PAD_RIGHT;

        case INPUT_CROSS:
            return PAD_CROSS;

        case INPUT_CIRCLE:
            return PAD_CIRCLE;

        case INPUT_START:
            return PAD_START;

        case INPUT_SELECT:
            return PAD_SELECT;

        default:
            return 0;
    }
}

int input_down(int button)
{
    unsigned short mask;

    if (!initialized)
        return 0;

    mask = get_mask(button);

    if (mask == 0)
        return 0;

    return ((current_buttons & mask) == 0);
}

int input_pressed(int button)
{
    unsigned short mask;

    if (!initialized)
        return 0;

    mask = get_mask(button);

    if (mask == 0)
        return 0;

    /*
     * Transicao:
     *
     * antes = solto
     * agora = pressionado
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
}
