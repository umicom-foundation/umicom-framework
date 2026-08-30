/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_guide.c
 *
 * PURPOSE:
 *   Validate represent user-created horizontal and vertical design guides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/guide.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadGuide item;CHECK(umi_rad_guide_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_guide_is_valid(&item));return 0;}
