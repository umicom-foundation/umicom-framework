/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_ruler.c
 *
 * PURPOSE:
 *   Validate describe design-time rulers and origin offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/ruler.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadRuler item;CHECK(umi_rad_ruler_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_ruler_is_valid(&item));return 0;}
