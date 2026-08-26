/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_snap_policy.c
 *
 * PURPOSE:
 *   Validate configure grid, guide and component snapping tolerance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/snap_policy.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadSnapPolicy item;CHECK(umi_rad_snap_policy_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_snap_policy_is_valid(&item));return 0;}
