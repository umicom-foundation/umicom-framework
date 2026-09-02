/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_responsive_variant.c
 *
 * PURPOSE:
 *   Validate describe per-breakpoint component geometry and visibility overrides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/responsive_variant.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadResponsiveVariant item;CHECK(umi_rad_responsive_variant_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_responsive_variant_is_valid(&item));return 0;}
