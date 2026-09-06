#include "input.h"

#include <kernel.h>
#include <sifrpc.h>
#include <libpad.h>
#include <malloc.h>
#include <string.h>

#define PAD_PORT 0
#define PAD_SLOT 0

static unsigned char *pad_buffer = NULL;
static struct padButtonStatus pad_status;

static InputState current;

static unsigned short previous_buttons = 0;

static int pad_open = 0;
static int initialized = 0;

static unsigned short pad_buttons(void)
{
    /*
     * O libpad usa logica ativa em nivel baixo.
     * Convertemos para nivel alto para o programa.
     */
    return (unsigned short)(0xFFFF ^ pad_status.btns);
}

static int just_pressed(unsigned short buttons,
                        unsigned short mask)
{
    return ((buttons & mask) != 0) &&
           ((previous_buttons & mask) == 0);
}

int input_init(void)
{
    /*
     * Inicializa comunicacao EE <-> IOP.
     */
    sceSifInitRpc(0);

    /*
     * Inicializa o sistema de controle.
     */
    if (!padInit(0))
    {
        return 0;
    }

    /*
     * Area de trabalho exigida pelo libpad.
     */
    pad_buffer = (unsigned char *)memalign(64, 256);

    if (pad_buffer == NULL)
    {
        padEnd();
        return 0;
    }

    memset(pad_buffer, 0, 256);
    memset(&pad_status, 0, sizeof(pad_status));
    memset(&current, 0, sizeof(current));

    /*
     * Abre porta 1, slot 1.
     */
    pad_open = padPortOpen(
        PAD_PORT,
        PAD_SLOT,
        pad_buffer
    );

    if (!pad_open)
    {
        free(pad_buffer);
        pad_buffer = NULL;
        padEnd();
        return 0;
    }

    /*
     * Tenta colocar o controle em modo DualShock.
     */
    padSetMainMode(
        PAD_PORT,
        PAD_SLOT,
        PAD_MMODE_DUALSHOCK,
        0
    );

    /*
     * Nao consideramos o controle pronto imediatamente.
     * O libpad precisa de algum tempo para inicializar.
     */
    previous_buttons = 0;

    initialized = 1;

    return 1;
}

void input_update(void)
{
    int state;
    unsigned short buttons;

    if (!initialized)
        return;

    /*
     * Verifica o estado atual do controle.
     */
    state = padGetState(PAD_PORT, PAD_SLOT);

    /*
     * Enquanto o controle estiver inicializando,
     * nao geramos comandos.
     */
    if (state != PAD_STATE_STABLE)
    {
        memset(&current, 0, sizeof(current));
        previous_buttons = 0;
        return;
    }

    /*
     * Leitura real do controle.
     */
    if (!padRead(PAD_PORT, PAD_SLOT, &pad_status))
    {
        memset(&current, 0, sizeof(current));
        return;
    }

    buttons = pad_buttons();

    /*
     * Limpa os eventos deste frame.
     */
    memset(&current, 0, sizeof(current));

    /*
     * Direcionais.
     */
    current.up =
        just_pressed(buttons, PAD_UP);

    current.down =
        just_pressed(buttons, PAD_DOWN);

    current.left =
        just_pressed(buttons, PAD_LEFT);

    current.right =
        just_pressed(buttons, PAD_RIGHT);

    /*
     * Botoes principais.
     */
    current.cross =
        just_pressed(buttons, PAD_CROSS);

    current.circle =
        just_pressed(buttons, PAD_CIRCLE);

    current.square =
        just_pressed(buttons, PAD_SQUARE);

    current.triangle =
        just_pressed(buttons, PAD_TRIANGLE);

    /*
     * START / SELECT.
     */
    current.start =
        just_pressed(buttons, PAD_START);

    current.select =
        just_pressed(buttons, PAD_SELECT);

    /*
     * Ombros.
     */
    current.l1 =
        just_pressed(buttons, PAD_L1);

    current.r1 =
        just_pressed(buttons, PAD_R1);

    current.l2 =
        just_pressed(buttons, PAD_L2);

    current.r2 =
        just_pressed(buttons, PAD_R2);

    /*
     * Guarda o estado para detectar o proximo pressionamento.
     */
    previous_buttons = buttons;
}

void input_shutdown(void)
{
    if (pad_open)
    {
        padPortClose(PAD_PORT, PAD_SLOT);
        pad_open = 0;
    }

    if (pad_buffer != NULL)
    {
        free(pad_buffer);
        pad_buffer = NULL;
    }

    if (initialized)
    {
        padEnd();
        initialized = 0;
    }

    memset(&current, 0, sizeof(current));
    previous_buttons = 0;
}

const InputState *input_get(void)
{
    return &current;
}
