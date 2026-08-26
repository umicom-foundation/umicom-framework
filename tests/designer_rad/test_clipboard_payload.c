/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_clipboard_payload.c
 *
 * PURPOSE:
 *   Validate represent semantic copied components without serialising toolkit objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/clipboard_payload.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadClipboardPayload p;UmiRadComponentInstance i;CHECK(umi_rad_clipboard_payload_init(&p)==UMI_STATUS_OK);CHECK(umi_rad_component_instance_init(&i)==UMI_STATUS_OK);CHECK(umi_rad_clipboard_payload_add(&p,&i)==UMI_STATUS_OK);CHECK(p.count==1U);return 0;}
