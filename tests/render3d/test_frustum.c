/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/render3d/test_frustum.c
 *
 * PURPOSE:
 *   Verify visible, culled and intersecting bounds with invalid-plane atomicity.
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
#include <stdio.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d\n",__LINE__);return 1;} }while(0)
int main(void)
{
    UmiRender3dPlane planes[6]={{{1,0,0},1},{{-1,0,0},1},{{0,1,0},1},{{0,-1,0},1},{{0,0,1},1},{{0,0,-1},1}};
    UmiRender3dBounds b={{-.5,-.5,-.5},{.5,.5,.5}};
    bool visible=false;
    CHECK(UmiRender3dBoundsVisible(&b,planes,&visible)==UMI_STATUS_OK && visible);
    b.minimum[0]=2;b.maximum[0]=3;
    CHECK(UmiRender3dBoundsVisible(&b,planes,&visible)==UMI_STATUS_OK && !visible);
    b.minimum[0]=1;
    CHECK(UmiRender3dBoundsVisible(&b,planes,&visible)==UMI_STATUS_OK && visible);
    b.minimum[0]=.5;
    CHECK(UmiRender3dBoundsVisible(&b,planes,&visible)==UMI_STATUS_OK && visible);
    planes[5].distance=NAN;visible=true;
    CHECK(UmiRender3dBoundsVisible(&b,planes,&visible)==UMI_STATUS_INVALID_ARGUMENT && visible);
    planes[5].distance=1;planes[5].normal[2]=0;
    CHECK(UmiRender3dBoundsVisible(&b,planes,&visible)==UMI_STATUS_INVALID_ARGUMENT && visible);
    b.minimum[0]=4;
    CHECK(UmiRender3dBoundsVisible(&b,planes,&visible)==UMI_STATUS_INVALID_ARGUMENT && visible);
    return 0;
}
