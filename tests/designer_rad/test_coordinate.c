/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_coordinate.c
 *
 * PURPOSE:
 *   Validate provide deterministic integer designer coordinates and translation operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/coordinate.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadCoordinate p=umi_rad_coordinate_translate(umi_rad_coordinate_make(2,3),4,-1);CHECK(p.x==6&&p.y==2);return 0;}
