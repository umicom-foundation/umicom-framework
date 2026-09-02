/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_insertion_marker.c
 *
 * PURPOSE:
 *   Validate represent insertion feedback within ordered containers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/insertion_marker.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadInsertionMarker item;CHECK(umi_rad_insertion_marker_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_insertion_marker_is_valid(&item));return 0;}
