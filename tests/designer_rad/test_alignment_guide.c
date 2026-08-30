/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_alignment_guide.c
 *
 * PURPOSE:
 *   Validate represent alignment evidence between visual components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/alignment_guide.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadAlignmentGuide item;CHECK(umi_rad_alignment_guide_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_alignment_guide_is_valid(&item));return 0;}
