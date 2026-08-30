/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/components/surface.h
 *
 * PURPOSE:
 *   Describe reusable drawing/chart/media surface dimensions and refresh state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */

#ifndef UMICOM_UI_COMPONENTS_SURFACE_H
#define UMICOM_UI_COMPONENTS_SURFACE_H
#include <stdint.h>
typedef struct UmiUiSurface { int32_t width; int32_t height; uint64_t revision; } UmiUiSurface;
UmiUiSurface umi_ui_surface_default(void);
#endif
