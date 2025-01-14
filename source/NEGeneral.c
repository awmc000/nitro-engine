// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <nds/arm9/postest.h>

#include "NEMain.h"
#include "NEMath.h"

/// @file NEGeneral.c

const char NE_VersioString[] =
    "Nitro Engine - Version " NITRO_ENGINE_VERSION_STRING " - "
    "(C) 2008-2011, 2019, 2022 Antonio Nino Diaz";

static bool NE_UsingConsole;
bool NE_TestTouch;
static int NE_screenratio;
static uint32_t NE_viewport;
static u8 NE_Screen;
bool NE_Dual;

NE_Input ne_input;

static bool ne_inited = false;

static SpriteEntry *NE_Sprites; // 2D sprites used for Dual 3D mode

static int ne_znear, ne_zfar;
static int fov;

static int ne_main_screen = 1; // 1 = top, 0 = bottom

void NE_End(void)
{
    if (!ne_inited)
        return;

    // Hide BG0
    REG_DISPCNT &= ~(DISPLAY_BG0_ACTIVE | ENABLE_3D);

    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);
    if (NE_Dual)
    {
        vramSetBankC(VRAM_C_LCD);
        vramSetBankD(VRAM_D_LCD);

        free(NE_Sprites);
    }
    else if (GFX_CONTROL & GL_CLEAR_BMP)
    {
        NE_ClearBMPEnable(false);
    }

    vramSetBankE(VRAM_E_LCD);
    if (NE_UsingConsole)
    {
        vramSetBankF(VRAM_F_LCD);
        NE_UsingConsole = false;
    }

    NE_GUISystemEnd();
    NE_SpriteSystemEnd();
    NE_PhysicsSystemEnd();
    NE_ModelSystemEnd();
    NE_AnimationSystemEnd();
    NE_TextResetSystem();
    NE_TextureSystemEnd();
    NE_CameraSystemEnd();
    NE_SpecialEffectSet(0);

    //Power off 3D hardware
    powerOff(POWER_3D_CORE | POWER_MATRIX);

    NE_DebugPrint("Nitro Engine disabled");

    ne_inited = false;
}

void NE_Viewport(int x1, int y1, int x2, int y2)
{
    // Start calculating screen ratio in f32 format
    ne_div_start((x2 - x1 + 1) << 12, (y2 - y1 + 1));

    // Save viewport
    NE_viewport = x1 | (y1 << 8) | (x2 << 16) | (y2 << 24);
    GFX_VIEWPORT = NE_viewport;

    MATRIX_CONTROL = GL_PROJECTION; // New projection matix for this viewport
    MATRIX_IDENTITY = 0;

    int fovy = fov * DEGREES_IN_CIRCLE / 360;
    NE_screenratio = ne_div_result();
    gluPerspectivef32(fovy, NE_screenratio, ne_znear, ne_zfar);

    MATRIX_CONTROL = GL_MODELVIEW;
}

void NE_MainScreenSetOnTop(void)
{
    ne_main_screen = 1;
}

void NE_MainScreenSetOnBottom(void)
{
    ne_main_screen = 0;
}

int NE_MainScreenIsOnTop(void)
{
    return ne_main_screen;
}

void NE_SwapScreens(void)
{
    ne_main_screen ^= 1;
}

void NE_SetFov(int fovValue)
{
    fov = fovValue;
}

static void ne_systems_end_all(void)
{
    NE_GUISystemEnd();
    NE_SpriteSystemEnd();
    NE_PhysicsSystemEnd();
    NE_ModelSystemEnd();
    NE_AnimationSystemEnd();
    NE_TextResetSystem();
    NE_TextureSystemEnd();
    NE_CameraSystemEnd();
    NE_SpecialEffectSet(0);
}

static int ne_systems_reset_all(NE_VRAMBankFlags vram_banks)
{
    // Default number of objects for everyting.
    if (NE_CameraSystemReset(0) != 0)
        goto cleanup;
    if (NE_PhysicsSystemReset(0) != 0)
        goto cleanup;
    if (NE_SpriteSystemReset(0) != 0)
        goto cleanup;
    if (NE_GUISystemReset(0) != 0)
        goto cleanup;
    if (NE_ModelSystemReset(0) != 0)
        goto cleanup;
    if (NE_AnimationSystemReset(0) != 0)
        goto cleanup;
    if (NE_TextureSystemReset(0, 0, vram_banks) != 0)
        goto cleanup;

    NE_TextPriorityReset();

    return 0;

cleanup:
    ne_systems_end_all();
    return -1;
}

static void ne_init_registers(void)
{
    // Power all 3D and 2D. Hide 3D screen during init
    powerOn(POWER_ALL);

    videoSetMode(0);

    vramSetBankE(VRAM_E_TEX_PALETTE);

    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    // Clear the FIFO
    GFX_STATUS |= (1 << 29);

    GFX_FLUSH = 0;
    GFX_FLUSH = 0;

    NE_MainScreenSetOnTop();
    REG_POWERCNT |= POWER_SWAP_LCDS;

    glResetMatrixStack();

    GFX_CONTROL = GL_TEXTURE_2D | GL_ANTIALIAS | GL_BLEND;

    GFX_ALPHA_TEST = 0;

    NE_ClearColorSet(NE_Black, 31, 63);
    NE_FogEnableBackground(false);

    GFX_CLEAR_DEPTH = GL_MAX_DEPTH;

    MATRIX_CONTROL = GL_TEXTURE;
    MATRIX_IDENTITY = 0;

    MATRIX_CONTROL = GL_PROJECTION;
    MATRIX_IDENTITY = 0;

    // Shininess table used for specular lighting
    NE_ShininessTableGenerate(NE_SHININESS_CUBIC);

    // setup default material properties
    NE_MaterialSetDefaultPropierties(RGB15(20, 20, 20), RGB15(16, 16, 16),
                                     RGB15(8, 8, 8), RGB15(5, 5, 5),
                                     false, true);

    // Turn off some things...
    for (int i = 0; i < 4; i++)
        NE_LightOff(i);

    GFX_COLOR = 0;
    GFX_POLY_FORMAT = 0;

    for (int i = 0; i < 8; i++)
        NE_OutliningSetColor(i, 0);

    ne_znear = floattof32(0.1);
    ne_zfar = floattof32(40.0);
    fov = 70;
    NE_Viewport(0, 0, 255, 191);

    MATRIX_CONTROL = GL_MODELVIEW;
    MATRIX_IDENTITY = 0;

    // Ready!!

    videoSetMode(MODE_0_3D);
}

void NE_UpdateInput(void)
{
    ne_input.kdown = keysDown();
    ne_input.kheld = keysHeld();
    ne_input.kup = keysUp();

    if (ne_input.kheld & KEY_TOUCH)
        touchRead(&ne_input.touch);
}

int NE_Init3D(void)
{
    if (ne_inited)
        NE_End();

    if (ne_systems_reset_all(NE_VRAM_ABCD) != 0)
        return -1;

    NE_UpdateInput();

    ne_init_registers();

    ne_inited = true;
    NE_Dual = false;

    NE_DebugPrint("Nitro Engine initialized in normal 3D mode");

    return 0;
}

int NE_InitDual3D(void)
{
    if (ne_inited)
        NE_End();

    NE_Sprites = calloc(128, sizeof(SpriteEntry));
    if (NE_Sprites == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return -1;
    }

    if (ne_systems_reset_all(NE_VRAM_AB) != 0)
    {
        free(NE_Sprites);
        return -2;
    }

    // Reset sprites
    for (int i = 0; i < 128; i++)
        NE_Sprites[i].attribute[0] = ATTR0_DISABLED;

    int i = 0;
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            NE_Sprites[i].attribute[0] = ATTR0_BMP | ATTR0_SQUARE | (64 * y);
            NE_Sprites[i].attribute[1] = ATTR1_SIZE_64 | (64 * x);
            NE_Sprites[i].attribute[2] = ATTR2_ALPHA(1) | (8 * 32 * y)
                                       | (8 * x);
            i++;
        }
    }

    NE_UpdateInput();

    ne_init_registers();

    videoSetModeSub(0);

    REG_BG2CNT_SUB = BG_BMP16_256x256;
    REG_BG2PA_SUB = 1 << 8;
    REG_BG2PB_SUB = 0;
    REG_BG2PC_SUB = 0;
    REG_BG2PD_SUB = 1 << 8;
    REG_BG2X_SUB = 0;
    REG_BG2Y_SUB = 0;

    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);

    videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE |
                    DISPLAY_SPR_2D_BMP_256);

    ne_inited = true;
    NE_Dual = true;

    NE_DebugPrint("Nitro Engine initialized in dual 3D mode");

    return 0;
}

void NE_InitConsole(void)
{
    if (!ne_inited)
        return;

    NE_UsingConsole = true;

    videoSetMode(MODE_0_3D | DISPLAY_BG1_ACTIVE);

    vramSetBankF(VRAM_F_MAIN_BG);

    BG_PALETTE[255] = 0xFFFF;

    // Use BG 1 for text, set to highest priority
    REG_BG1CNT = BG_MAP_BASE(4) | BG_PRIORITY(0);

    // Set BG 0 (3D background) to be a lower priority than BG 1
    REG_BG0CNT = BG_PRIORITY(1);

    consoleInit(0, 1, BgType_Text4bpp, BgSize_T_256x256, 4, 0, true, true);
}

void NE_SetConsoleColor(u32 color)
{
    BG_PALETTE[255] = color;
}

void NE_Process(NE_Voidfunc drawscene)
{
    NE_UpdateInput();

    if (ne_main_screen == 1)
        REG_POWERCNT |= POWER_SWAP_LCDS;
    else
        REG_POWERCNT &= ~POWER_SWAP_LCDS;

    NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

    GFX_VIEWPORT = NE_viewport;

    MATRIX_CONTROL = GL_PROJECTION;
    MATRIX_IDENTITY = 0;
    gluPerspectivef32(fov * DEGREES_IN_CIRCLE / 360, NE_screenratio,
                      ne_znear, ne_zfar);

    MATRIX_CONTROL = GL_MODELVIEW;
    MATRIX_IDENTITY = 0;

    NE_AssertPointer(drawscene, "NULL function pointer");
    drawscene();

    GFX_FLUSH = GL_TRANS_MANUALSORT;
}

void NE_ProcessDual(NE_Voidfunc mainscreen, NE_Voidfunc subscreen)
{
    NE_AssertPointer(mainscreen, "NULL function pointer (main screen)");
    NE_AssertPointer(subscreen, "NULL function pointer (sub screen)");

    NE_UpdateInput();

    if (NE_Screen == ne_main_screen)
        REG_POWERCNT |= POWER_SWAP_LCDS;
    else
        REG_POWERCNT &= ~POWER_SWAP_LCDS;

    if (NE_Screen == 1)
    {
        if (NE_UsingConsole)
        {
            REG_BG1CNT = BG_MAP_BASE(4) | BG_PRIORITY(0);
            REG_BG0CNT = BG_PRIORITY(1);
        }

        vramSetBankC(VRAM_C_SUB_BG);
        vramSetBankD(VRAM_D_LCD);

        REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
                       | DCAP_BANK(DCAP_BANK_VRAM_D)
                       | DCAP_MODE(DCAP_MODE_A)
                       | DCAP_SRC_A(DCAP_SRC_A_COMPOSITED)
                       | DCAP_ENABLE;
    }
    else
    {
        if (NE_UsingConsole)
        {
            REG_BG1CNT = BG_PRIORITY(1);
            REG_BG0CNT = BG_PRIORITY(0);
        }

        vramSetBankC(VRAM_C_LCD);
        vramSetBankD(VRAM_D_SUB_SPRITE);

        REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
                       | DCAP_BANK(DCAP_BANK_VRAM_C)
                       | DCAP_MODE(DCAP_MODE_A)
                       | DCAP_SRC_A(DCAP_SRC_A_COMPOSITED)
                       | DCAP_ENABLE;
    }

    NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

    NE_Viewport(0, 0, 255, 191);

    MATRIX_IDENTITY = 0;

    if (NE_Screen == 1)
        mainscreen();
    else
        subscreen();

    GFX_FLUSH = GL_TRANS_MANUALSORT;

    dmaCopy(NE_Sprites, OAM_SUB, 128 * sizeof(SpriteEntry));
}

void NE_ClippingPlanesSetI(int znear, int zfar)
{
    NE_Assert(znear < zfar, "znear must be smaller than zfar");
    ne_znear = znear;
    ne_zfar = zfar;
}

void NE_AntialiasEnable(bool value)
{
    if (value)
        GFX_CONTROL |= GL_ANTIALIAS;
    else
        GFX_CONTROL &= ~GL_ANTIALIAS;
}

int NE_GetPolygonCount(void)
{
    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    return GFX_POLYGON_RAM_USAGE;
}

int NE_GetVertexCount(void)
{
    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    return GFX_VERTEX_RAM_USAGE;
}

static int NE_Effect = NE_NONE;
static int NE_lastvbladd = 0;
static bool NE_effectpause;
#define NE_NOISEPAUSE_SIZE 512
static int *ne_noisepause;
static int ne_cpucount;
static int ne_noise_value = 0xF;
static int ne_sine_mult = 10, ne_sine_shift = 9;

void NE_VBLFunc(void)
{
    if (!ne_inited)
        return;

    if (NE_Effect == NE_NOISE || NE_Effect == NE_SINE)
    {
        if (!NE_effectpause)
            NE_lastvbladd = (NE_lastvbladd + 1) & (NE_NOISEPAUSE_SIZE - 1);
    }

    NE_Screen ^= 1;
}

void NE_SpecialEffectPause(bool pause)
{
    if (NE_Effect == 0)
        return;

    if (pause)
    {
        ne_noisepause = malloc(sizeof(int) * NE_NOISEPAUSE_SIZE);
        if (ne_noisepause == NULL)
        {
            NE_DebugPrint("Not enough memory");
            return;
        }

        for (int i = 0; i < NE_NOISEPAUSE_SIZE; i++)
        {
            ne_noisepause[i] = (rand() & ne_noise_value)
                             - (ne_noise_value >> 1);
        }
    }
    else
    {
        if (ne_noisepause != NULL)
        {
            free(ne_noisepause);
            ne_noisepause = NULL;
        }
    }

    NE_effectpause = pause;
}

void NE_HBLFunc(void)
{
    s16 angle;
    int val;

    if (!ne_inited)
        return;

    // This counter is used to estimate CPU usage
    ne_cpucount++;

    // Fix a problem with the first line when using effects
    int vcount = REG_VCOUNT;
    if (vcount == 262)
        vcount = 0;

    switch (NE_Effect)
    {
        case NE_NOISE:
            if (NE_effectpause && ne_noisepause)
                val = ne_noisepause[vcount & (NE_NOISEPAUSE_SIZE - 1)];
            else
                val = (rand() & ne_noise_value) - (ne_noise_value >> 1);
            REG_BG0HOFS = val;
            break;

        case NE_SINE:
            angle = (vcount + NE_lastvbladd) * ne_sine_mult;
            REG_BG0HOFS = sinLerp(angle << 6) >> ne_sine_shift;
            break;

        default:
            break;
    }
}

void NE_SpecialEffectNoiseConfig(int value)
{
    ne_noise_value = value;
}

void NE_SpecialEffectSineConfig(int mult, int shift)
{
    ne_sine_mult = mult;
    ne_sine_shift = shift;
}

void NE_SpecialEffectSet(NE_SpecialEffects effect)
{
    NE_Effect = effect;

    if (effect == NE_NONE)
        REG_BG0HOFS = 0;
}

static int NE_CPUPercent;

void NE_WaitForVBL(NE_UpdateFlags flags)
{
    if (flags & NE_UPDATE_GUI)
        NE_GUIUpdate();
    if (flags & NE_UPDATE_ANIMATIONS)
        NE_ModelAnimateAll();
    if (flags & NE_UPDATE_PHYSICS)
        NE_PhysicsUpdateAll();

    NE_CPUPercent = div32(ne_cpucount * 100, 263);
    if (flags & NE_CAN_SKIP_VBL)
    {
        if (NE_CPUPercent > 100)
        {
            ne_cpucount = 0;
            return;

            // REG_DISPSTAT & DISP_IN_VBLANK
        }
    }

    swiWaitForVBlank();
    ne_cpucount = 0;
}

int NE_GetCPUPercent(void)
{
    return NE_CPUPercent;
}

bool NE_GPUIsRendering(void)
{
    if (REG_VCOUNT > 190 && REG_VCOUNT < 214)
        return false;

    return true;
}

#ifdef NE_DEBUG
static void (*ne_userdebugfn)(const char *) = NULL;

void __ne_debugoutputtoconsole(const char *text)
{
    printf(text);
}

void __NE_debugprint(const char *text)
{
    if (!ne_inited)
        return;
    if (ne_userdebugfn)
        ne_userdebugfn(text);
}

void NE_DebugSetHandler(void (*fn)(const char *))
{
    ne_userdebugfn = fn;
}

void NE_DebugSetHandlerConsole(void)
{
    NE_InitConsole();
    ne_userdebugfn = __ne_debugoutputtoconsole;
}
#endif

static int ne_vertexcount;

void NE_TouchTestStart(void)
{
    // Hide what we are going to draw
    GFX_VIEWPORT = 255 | (255 << 8) | (255 << 16) | (255 << 24);

    // Save current state
    MATRIX_CONTROL = GL_MODELVIEW;
    MATRIX_PUSH = 0;
    MATRIX_CONTROL = GL_PROJECTION;
    MATRIX_PUSH = 0;

    // Setup temporary render environment
    MATRIX_IDENTITY = 0;

    int temp[4] = {
        NE_viewport & 0xFF,
        (NE_viewport >> 8) & 0xFF,
        (NE_viewport >> 16) & 0xFF,
        (NE_viewport >> 24) & 0xFF
    };

    // Render only what is below the cursor
    gluPickMatrix(ne_input.touch.px, 191 - ne_input.touch.py, 3, 3, temp);
    gluPerspectivef32(fov * DEGREES_IN_CIRCLE / 360, NE_screenratio,
                      ne_znear, ne_zfar);

    MATRIX_CONTROL = GL_MODELVIEW;

    NE_Assert(!NE_TestTouch, "Test already active");

    NE_TestTouch = true;
}

void NE_TouchTestObject(void)
{
    NE_Assert(NE_TestTouch, "No active test");

    // Wait for the position test to finish
    while (PosTestBusy());

    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    // Save the vertex ram count
    ne_vertexcount = NE_GetVertexCount();
}

int NE_TouchTestResult(void)
{
    NE_Assert(NE_TestTouch, "No active test");

    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    // Wait for the position test to finish
    while (PosTestBusy());

    // If a polygon was drawn
    if (NE_GetVertexCount() > ne_vertexcount)
        return PosTestWresult();

    return -1;
}

void NE_TouchTestEnd(void)
{
    NE_Assert(NE_TestTouch, "No active test");

    NE_TestTouch = false;

    // Reset the viewport
    GFX_VIEWPORT = NE_viewport;

    // Restore previous state
    MATRIX_CONTROL = GL_PROJECTION;
    MATRIX_POP = 1;
    MATRIX_CONTROL = GL_MODELVIEW;
    MATRIX_POP = 1;
}
