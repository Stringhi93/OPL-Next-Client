#include <stdio.h>
#include <debug.h>
#include <sifrpc.h>

#include "input.h"
#include "ui.h"
#include "network.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    SifInitRpc(0);

    init_scr();
    scr_clear();

    input_init();

    network_init();

    ui_init();

    while (1)
    {
        input_update();
        ui_update();
        ui_render();
    }

    network_shutdown();
    input_shutdown();

    return 0;
}
