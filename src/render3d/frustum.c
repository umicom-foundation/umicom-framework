/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/render3d/frustum.c
 *
 * PURPOSE:
 *   Cull bounds outside view planes while refusing invalid geometry before publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/render3d/frustum.h"
#include <math.h>
UmiStatus UmiRender3dBoundsVisible(const UmiRender3dBounds *bounds,
    const UmiRender3dPlane planes[6], bool *out_visible)
{
    if (!bounds || !planes || !out_visible) return UMI_STATUS_INVALID_ARGUMENT;
    for (unsigned axis=0;axis<3;++axis)
        if (!isfinite(bounds->minimum[axis]) || !isfinite(bounds->maximum[axis]) ||
            bounds->minimum[axis]>bounds->maximum[axis]) return UMI_STATUS_INVALID_ARGUMENT;
    bool visible=true;
    for (unsigned p=0;p<6;++p) {
        double distance=planes[p].distance;
        bool nonzero=false;
        if (!isfinite(distance)) return UMI_STATUS_INVALID_ARGUMENT;
        for(unsigned axis=0;axis<3;++axis) {
            double n=planes[p].normal[axis];
            if(!isfinite(n)) return UMI_STATUS_INVALID_ARGUMENT;
            nonzero |= n != 0;
            double vertex=n>=0 ? bounds->maximum[axis] : bounds->minimum[axis];
            distance += n*vertex;
            if(!isfinite(distance)) return UMI_STATUS_INVALID_ARGUMENT;
        }
        if(!nonzero) return UMI_STATUS_INVALID_ARGUMENT;
        if(distance<0) visible=false;
    }
    *out_visible=visible;
    return UMI_STATUS_OK;
}
