/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_size.c
 *
 * PURPOSE:
 *   Validate provide bounded designer size values and clamp operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/size.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDesignerSize s=umi_rad_size_clamp(umi_rad_size_make(50,500),umi_rad_size_make(100,100),umi_rad_size_make(400,400));CHECK(s.width==100&&s.height==400);return 0;}
