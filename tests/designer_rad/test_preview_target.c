/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_preview_target.c
 *
 * PURPOSE:
 *   Validate describe GTK4, Qt6, Native Web or abstract-device preview targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/preview_target.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadPreviewTarget item;CHECK(umi_rad_preview_target_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_preview_target_is_valid(&item));return 0;}
