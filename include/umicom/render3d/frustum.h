/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/render3d/frustum.h
 *
 * PURPOSE:
 *   Expose conservative axis-aligned bounds classification for CPU render visibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_RENDER3D_FRUSTUM_H
#define UMICOM_RENDER3D_FRUSTUM_H
#include <stdbool.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRender3dBounds { double minimum[3]; double maximum[3]; } UmiRender3dBounds;
/* Interior satisfies normal dot point + distance >= 0; normals need not be
 * normalized. Caller supplies six inward-facing planes in world coordinates. */
typedef struct UmiRender3dPlane { double normal[3]; double distance; } UmiRender3dPlane;
/* Return possibly visible, including intersecting/touching boxes. This
 * conservative test can retain a box near a frustum corner; it never claims
 * exact polyhedron intersection. No GPU, camera matrix or scene mutation.
 * Reject nonfinite/inverted bounds, zero normals and nonfinite intermediate
 * arithmetic. Refusal preserves out_visible. Caller owns stable disjoint
 * inputs and output for this synchronous, allocation-free call. */
UmiStatus UmiRender3dBoundsVisible(const UmiRender3dBounds *bounds,
    const UmiRender3dPlane planes[6], bool *out_visible);
#ifdef __cplusplus
}
#endif
#endif
