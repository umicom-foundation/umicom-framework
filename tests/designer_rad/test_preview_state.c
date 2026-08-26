/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_preview_state.c
 *
 * PURPOSE:
 *   Validate record renderer-neutral preview health and diagnostic counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/preview_state.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadPreviewState item;CHECK(umi_rad_preview_state_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_preview_state_is_valid(&item));return 0;}
