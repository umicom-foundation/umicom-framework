/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_event_binding.c
 *
 * PURPOSE:
 *   Validate bind a semantic component event to a Framework command identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/event_binding.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadEventBinding item;CHECK(umi_rad_event_binding_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_event_binding_is_valid(&item));return 0;}
