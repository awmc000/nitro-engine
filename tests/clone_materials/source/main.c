// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a3pal32_pal_bin.h"
#include "a3pal32_tex_bin.h"

#define NUM_CLONES 5

NE_Material *Material[NUM_CLONES];
NE_Palette *Palette;

int main(void)
{
    // This is needed for special screen effects
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_VBLANK, NE_HBLFunc);

    // Init console and Nitro Engine
    NE_Init3D();
    // Use banks A and B for teapots. libnds uses bank C for the demo text
    // console.
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // This is needed to print text
    consoleDemoInit();

    for (int i = 0; i < NUM_CLONES; i++)
        Material[i] = NE_MaterialCreate();
    Palette = NE_PaletteCreate();

    int total_tex_mem = NE_TextureFreeMem();
    int total_pal_mem = NE_PaletteFreeMem();

    NE_MaterialTexLoad(Material[0],
                       NE_A3PAL32, // Texture type
                       64, 200,    // Width, height (in pixels)
                       NE_TEXGEN_TEXCOORD, (u8 *)a3pal32_tex_bin);
    NE_PaletteLoad(Palette, (u16 *)a3pal32_pal_bin, 32, NE_A3PAL32);
    NE_MaterialSetPalette(Material[0], Palette);

    for (int i = 1; i < NUM_CLONES; i++)
        NE_MaterialClone(Material[0], Material[i]);

    int remaining_tex_mem = NE_TextureFreeMem();
    int remaining_pal_mem = NE_PaletteFreeMem();

    printf("Total:     %6d | %6d\n"
           "Remaining: %6d | %6d\n",
           total_tex_mem, total_pal_mem,
           remaining_tex_mem, remaining_pal_mem);

    // Delete all materials but one, so that the texture isn't freed yet
    for (int i = 0; i < NUM_CLONES - 1; i++)
        NE_MaterialDelete(Material[i]);

    if (remaining_tex_mem != NE_TextureFreeMem())
        printf("Texture memory wrongly freed\n");

    if (remaining_pal_mem != NE_PaletteFreeMem())
        printf("Palette memory wrongly freed\n");

    // Delete the last material so that the texture is freed
    NE_MaterialDelete(Material[NUM_CLONES - 1]);
    NE_PaletteDelete(Palette);

    if (total_tex_mem != NE_TextureFreeMem())
        printf("Texture memory not freed\n");

    if (total_pal_mem != NE_PaletteFreeMem())
        printf("Palette memory not freed\n");

    printf("Tests finished\n");

    while (1)
        NE_WaitForVBL(0);

    return 0;
}
