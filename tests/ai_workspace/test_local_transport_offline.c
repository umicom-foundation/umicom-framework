/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_local_transport_offline.c
 *
 * PURPOSE:
 *   Verify optional local transport refusal preserves caller output without initializing HTTP.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_transport_internal.h"
#include <stdint.h>
#include <stdio.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(void)
{
    UmiAiLocalTransport *output=(UmiAiLocalTransport *)(uintptr_t)1U,*before=output;
    CHECK(UmiAiLocalTransportCreate(&output)==UMI_STATUS_UNAVAILABLE && output==before);
    CHECK(UmiAiLocalTransportCreate(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiLocalTransportDestroy(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    output=NULL;CHECK(UmiAiLocalTransportDestroy(&output)==UMI_STATUS_OK);
    size_t count=77U;
    CHECK(UmiAiLocalTransportRequestCount(NULL,&count)==UMI_STATUS_INVALID_ARGUMENT && count==77U);
    CHECK(AwLocalTransportRetain(NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(AwLocalTransportRelease(NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;
}
