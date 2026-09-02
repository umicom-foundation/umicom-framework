/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_alignment_guide.c
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
#include "umicom/designer/visual_designer/alignment_guide.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadAlignmentGuide item;CHECK(umi_rad_alignment_guide_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_alignment_guide_is_valid(&item));return 0;}
