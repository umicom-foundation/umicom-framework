/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/render3d/types.c
 *
 * PURPOSE:
 *   Implement safe default values for toolkit-neutral 3D records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/render3d/types.h"

/* Construct identity rotation and unit scale for a newly created scene node. */
UmiRender3dTransform umi_render3d_transform_identity(void)
{
    UmiRender3dTransform transform = {0};
    /* Quaternion W=1 and unit scale describe an unchanged object. */
    transform.rotation_w = 1.0;
    transform.scale_x = 1.0;
    transform.scale_y = 1.0;
    transform.scale_z = 1.0;
    return transform;
}
