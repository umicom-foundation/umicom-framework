/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_distribution_guide.c
 *
 * PURPOSE:
 *   Validate represent equal-spacing evidence for multiple selected components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/distribution_guide.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDistributionGuide item;CHECK(umi_rad_distribution_guide_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_distribution_guide_is_valid(&item));return 0;}
