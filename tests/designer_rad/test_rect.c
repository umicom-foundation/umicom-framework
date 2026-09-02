/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_rect.c
 *
 * PURPOSE:
 *   Validate provide rectangle geometry used by selection, hit testing and layout editing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/rect.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDesignerRect a=umi_rad_rect_make(0,0,100,100),b=umi_rad_rect_make(90,90,20,20);UmiRadPoint p={50,50};CHECK(umi_rad_rect_contains(&a,p));CHECK(umi_rad_rect_intersects(&a,&b));return 0;}
