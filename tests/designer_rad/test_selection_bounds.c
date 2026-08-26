/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_selection_bounds.c
 *
 * PURPOSE:
 *   Validate calculate the bounding rectangle of a multi-component selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/selection_bounds.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadRect r[2]={{0,0,10,10},{10,5,20,10}};UmiRadSelectionBounds b;CHECK(umi_rad_selection_bounds_compute(r,2U,&b)==UMI_STATUS_OK);CHECK(b.bounds.width==30&&b.bounds.height==15);return 0;}
