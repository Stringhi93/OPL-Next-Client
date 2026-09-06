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
     * libpad reports buttons as active-low.
     * Convert them to active-high for the application.
     */
    return (unsigned short)(0xFFFF ^ pad_status.btns);
}

static int pressed(unsigned short buttons, unsigned short mask)
{
    return (buttons & mask) != 0;
}

static int just_pressed(unsigned short buttons, unsigned short mask)
{
    return ((buttons & mask) != 0) &&
           ((previous_buttons & mask) == 0);
}

int input_init(void)
{
    /*
     * Initialize SIF RPC because libpad communicates
     * with the IOP through RPC.
     */
    sceSifInitRpc(0);

    if (!padInit(0))
    {
        return 0;
    }

    /*
     * libpad requires an aligned 256-byte area.
     * 64-byte alignment is safe for current PS2SDK.
     */
    pad_buffer = (unsigned char *)memalign(64, 256);

    if (pad_buffer == NULL)
    {
        return 0;
    }

    memset(pad_buffer, 0, 256);
    memset(&current, 0, sizeof(current));

    pad_open = padPortOpen(
        PAD_PORT,
        PAD_SLOT,
        pad_buffer
    );

    if (!pad_open)
    {
        free(pad_buffer);
        pad_buffer = NULL;
        return 0;
    }

    /*
     * Request DualShock mode when available.
     */
    padSetMainMode(
        PAD_PORT,
        PAD_SLOT,
        PAD_MMODE_DUALSHOCK,
        0
    );

    initialized = 1;

    return 1;
}

void input_update(void)
{
    unsigned short buttons;

    if (!initialized)
        return;

    /*
     * Controller may still be initializing.
     */
    if (padGetState(PAD_PORT, PAD_SLOT) != PAD_STATE_STABLE)
    {
        memset(&current, 0, sizeof(current));
        previous_buttons = 0;
        return;
    }

    if (!padRead(PAD_PORT, PAD_SLOT, &pad_status))
    {
        memset(&current, 0, sizeof(current));
        return;
    }

    buttons = pad_buttons();

    memset(&current, 0, sizeof(current));

    /*
     * Directional buttons are reported continuously.
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
     * Face buttons.
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
     * System buttons.
     */
    current.start =
        just_pressed(buttons, PAD_START);

    current.select =
        just_pressed(buttons, PAD_SELECT);

    /*
     * Shoulder buttons.
     */
    current.l1 =
        just_pressed(buttons, PAD_L1);

    current.r1 =
        just_pressed(buttons, PAD_R1);

    current.l2 =
        just_pressed(buttons, PAD_L2);

    current.r2 =
        just_pressed(buttons, PAD_R2);

    previous_buttons = buttons;
}

void input_shutdown(void)
{
    if (pad_open)
    {
        padPortClose(PAD_PORT, PAD_SLOT);
        pad_open = 0;
    }

    if (pad_buffer)
    {
        free(pad_buffer);
        pad_buffer = NULL;
    }

    if (initialized)
    {
        padEnd();
        initialized = 0;
    }
}

const InputState *input_get(void)
{
    return &current;
}
