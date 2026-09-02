/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_component_catalogue_bridge.c
 *
 * PURPOSE:
 *   Validate map Design System component identifiers to canonical designer component types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/component_catalogue_bridge.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadComponentCatalogueBridge item;CHECK(umi_rad_component_catalogue_bridge_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_component_catalogue_bridge_is_valid(&item));return 0;}
