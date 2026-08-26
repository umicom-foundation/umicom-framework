/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_event_descriptor.c
 *
 * PURPOSE:
 *   Validate describe an event exposed by a semantic component.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/event_descriptor.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadEventDescriptor item;CHECK(umi_rad_event_descriptor_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_event_descriptor_is_valid(&item));return 0;}
