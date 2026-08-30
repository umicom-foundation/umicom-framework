/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_drop_target.c
 *
 * PURPOSE:
 *   Validate represent validated parent/slot destinations during component drag and drop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/drop_target.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDropTarget item;CHECK(umi_rad_drop_target_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_drop_target_is_valid(&item));return 0;}
