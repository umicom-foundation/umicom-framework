/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_validation_marker.c
 *
 * PURPOSE:
 *   Validate attach a validation severity/message to a component or property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/validation_marker.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadValidationMarker item;CHECK(umi_rad_validation_marker_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_validation_marker_is_valid(&item));return 0;}
