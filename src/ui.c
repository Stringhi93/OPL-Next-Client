#include "ui.h"
#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <gsKit.h>
#include <dmaKit.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 448

#define COLOR_BG       GS_SETREG_RGBAQ(8, 10, 16, 0x80, 0)
#define COLOR_PANEL    GS_SETREG_RGBAQ(20, 24, 34, 0x80, 0)
#define COLOR_PANEL2   GS_SETREG_RGBAQ(28, 34, 48, 0x80, 0)
#define COLOR_ACCENT   GS_SETREG_RGBAQ(40, 140, 255, 0x80, 0)
#define COLOR_ACCENT2  GS_SETREG_RGBAQ(70, 180, 255, 0x80, 0)
#define COLOR_WHITE    GS_SETREG_RGBAQ(235, 240, 250, 0x80, 0)
#define COLOR_TEXT     GS_SETREG_RGBAQ(190, 200, 215, 0x80, 0)
#define COLOR_LINE     GS_SETREG_RGBAQ(50, 60, 80, 0x80, 0)

static GSGLOBAL *gsGlobal = NULL;

static int selected = 0;

static const char *menu_items[] =
{
    "JOGOS",
    "REDE",
    "CONFIGURACOES"
};

#define MENU_COUNT 3

static void draw_rect(
    float x1,
    float y1,
    float x2,
    float y2,
    u64 color
)
{
    gsKit_prim_sprite(
        gsGlobal,
        x1,
        y1,
        x2,
        y2,
        1,
        color
    );
}

static void draw_background(void)
{
    draw_rect(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        COLOR_BG
    );
}

static void draw_header(void)
{
    draw_rect(
        0,
        0,
        SCREEN_WIDTH,
        58,
        COLOR_PANEL
    );

    draw_rect(
        0,
        57,
        SCREEN_WIDTH,
        60,
        COLOR_ACCENT
    );
}

static void draw_preview(void)
{
    /*
     * Area reservada para a capa do jogo.
     * Nesta V0.2 ainda nao carregamos imagens.
     */
    draw_rect(
        42,
        105,
        230,
        365,
        COLOR_PANEL
    );

    draw_rect(
        54,
        117,
        218,
        353,
        COLOR_PANEL2
    );

    /*
     * Pequeno detalhe visual no centro.
     */
    draw_rect(
        92,
        205,
        180,
        220,
        COLOR_ACCENT
    );

    draw_rect(
        105,
        225,
        167,
        230,
        COLOR_ACCENT2
    );
}

static void draw_menu(void)
{
    int i;

    for (i = 0; i < MENU_COUNT; i++)
    {
        float y = 125.0f + (float)(i * 70);

        if (i == selected)
        {
            draw_rect(
                270,
                y - 8,
                590,
                y + 45,
                COLOR_ACCENT
            );

            draw_rect(
                277,
                y - 1,
                583,
                y + 38,
                COLOR_PANEL2
            );
        }
        else
        {
            draw_rect(
                270,
                y - 8,
                590,
                y + 45,
                COLOR_PANEL
            );
        }
    }
}

static void draw_footer(void)
{
    draw_rect(
        0,
        410,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        COLOR_PANEL
    );

    draw_rect(
        0,
        409,
        SCREEN_WIDTH,
        411,
        COLOR_LINE
    );
}

void ui_init(void)
{
    /*
     * Inicializa o sistema DMA utilizado pelo gsKit.
     */
    dmaKit_init(
        D_CTRL_RELE_OFF,
        D_CTRL_MFD_OFF,
        D_CTRL_STS_UNSPEC,
        D_CTRL_STD_OFF,
        D_CTRL_RCYC_8,
        1 << DMA_CHANNEL_GIF
    );

    dmaKit_chan_init(DMA_CHANNEL_GIF);

    /*
     * NTSC 640x448.
     *
     * Escolhemos este modo inicialmente porque o teste
     * anterior foi feito em um PS2 real e apresentou boa
     * compatibilidade.
     */
    gsGlobal = gsKit_init_global();

    if (gsGlobal == NULL)
    {
        return;
    }

    gsGlobal->Mode = GS_MODE_NTSC;
    gsGlobal->Interlace = GS_INTERLACED;
    gsGlobal->Field = GS_FRAME;

    gsGlobal->Width = SCREEN_WIDTH;
    gsGlobal->Height = SCREEN_HEIGHT;

    gsGlobal->PSM = GS_PSM_CT16;
    gsGlobal->PSMZ = GS_PSMZ_16;

    gsGlobal->DoubleBuffering = GS_SETTING_ON;
    gsGlobal->ZBuffering = GS_SETTING_OFF;
    gsGlobal->Dithering = GS_SETTING_OFF;

    /*
     * Inicializa o framebuffer.
     */
    gsKit_init_screen(gsGlobal);

    selected = 0;
}

void ui_shutdown(void)
{
    if (gsGlobal != NULL)
    {
        gsKit_deinit_global(gsGlobal);
        gsGlobal = NULL;
    }
}

void ui_update(void)
{
    const InputState *input;

    input_update();

    input = input_get();

    if (input->up)
    {
        selected--;

        if (selected < 0)
        {
            selected = MENU_COUNT - 1;
        }
    }

    if (input->down)
    {
        selected++;

        if (selected >= MENU_COUNT)
        {
            selected = 0;
        }
    }

    /*
     * Nesta V0.2 o X ainda nao abre uma nova tela.
     * Apenas reservamos o comportamento para a proxima etapa.
     */
    if (input->cross)
    {
        /*
         * Futuramente:
         *
         * JOGOS          -> biblioteca
         * REDE           -> SMB
         * CONFIGURACOES  -> configuracoes
         */
    }
}

void ui_render(void)
{
    if (gsGlobal == NULL)
    {
        return;
    }

    /*
     * Limpa o framebuffer atual.
     */
    gsKit_clear(
        gsGlobal,
        COLOR_BG
    );

    draw_background();

    draw_header();

    draw_preview();

    draw_menu();

    draw_footer();

    /*
     * Executa a fila de comandos do GS.
     */
    gsKit_queue_exec(gsGlobal);

    /*
     * Troca o framebuffer sincronizado com o vídeo.
     */
    gsKit_sync_flip(gsGlobal);
}
