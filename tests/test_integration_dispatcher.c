/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_dispatcher.c
 *
 * PURPOSE:
 *   Exercise one Suite and Inter-Application Runtime Foundation behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/dispatcher.h"
#include <stdio.h>

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

/*
 * Exercise hit and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus hit(const UmiIntegrationEnvelope *e, void *u) {
    int *count=(int*)u;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (e==NULL||count==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ++*count; return UMI_STATUS_OK;
}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiIntegrationDispatcher d; UmiIntegrationEnvelope e; int count=0;
    umi_integration_dispatcher_init(&d); umi_integration_envelope_init(&e);
    CHECK(umi_integration_dispatcher_register(&d,"bank","ping",hit,&count)==UMI_STATUS_OK);
    (void)snprintf(e.source_application,sizeof(e.source_application),"studio");
    (void)snprintf(e.target_application,sizeof(e.target_application),"bank");
    (void)snprintf(e.topic,sizeof(e.topic),"ping");
    CHECK(umi_integration_dispatcher_dispatch(&d,&e)==UMI_STATUS_OK);
    CHECK(count==1);
    return 0;
}
