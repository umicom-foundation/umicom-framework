/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_insertion_marker.c
 *
 * PURPOSE:
 *   Validate represent insertion feedback within ordered containers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/insertion_marker.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadInsertionMarker item;CHECK(umi_rad_insertion_marker_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_insertion_marker_is_valid(&item));return 0;}
