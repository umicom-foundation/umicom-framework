/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_router.c
 *
 * PURPOSE:
 *   Exercise one Suite and Inter-Application Runtime Foundation behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/router.h"
#include <stdio.h>

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

int main(void) {
    UmiIntegrationRouter r; UmiIntegrationRoute x = {0};
    umi_integration_router_init(&r);
    (void)snprintf(x.source_application,sizeof(x.source_application),"*");
    (void)snprintf(x.target_application,sizeof(x.target_application),"bank");
    (void)snprintf(x.topic,sizeof(x.topic),"account.changed");
    x.kind=UMI_INTEGRATION_MESSAGE_EVENT;
    CHECK(umi_integration_router_add(&r,&x)==UMI_STATUS_OK);
    CHECK(umi_integration_router_resolve(&r,"trader","bank","account.changed",UMI_INTEGRATION_MESSAGE_EVENT)!=NULL);
    return 0;
}
