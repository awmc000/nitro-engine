// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_MAIN_H__
#define NE_MAIN_H__

/// @file   NEMain.h
/// @brief  Main header of Nitro Engine.

/// @defgroup global_defines Global definitions
///
/// Definitions related to debug features and version number of the library.
///
/// @{

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "NE2D.h"
#include "NEAnimation.h"
#include "NECamera.h"
#include "NEFAT.h"
#include "NEFormats.h"
#include "NEGeneral.h"
#include "NEGUI.h"
#include "NEModel.h"
#include "NEPalette.h"
#include "NEPhysics.h"
#include "NEPolygon.h"
#include "NEText.h"
#include "NETexture.h"

/// Major version of Nitro Engine
#define NITRO_ENGINE_MAJOR (0)
/// Minor version of Nitro Engine
#define NITRO_ENGINE_MINOR (8)
/// Patch version of Nitro Engine
#define NITRO_ENGINE_PATCH (2)

/// Full version of Nitro Engine
#define NITRO_ENGINE_VERSION ((NITRO_ENGINE_MAJOR << 16) |  \
                              (NITRO_ENGINE_MINOR << 8) |   \
                              (NITRO_ENGINE_PATCH))

/// String with the version of Nitro Engine
#define NITRO_ENGINE_VERSION_STRING "0.8.2"

/// @}

#ifdef __cplusplus
}
#endif

#endif // NE_MAIN_H__
