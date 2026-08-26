/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_distribution_guide.c
 *
 * PURPOSE:
 *   Validate represent equal-spacing evidence for multiple selected components.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/distribution_guide.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDistributionGuide item;CHECK(umi_rad_distribution_guide_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_distribution_guide_is_valid(&item));return 0;}
