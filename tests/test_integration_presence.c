/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_presence.c
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

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/presence.h"

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

int main(void) {
    UmiIntegrationPresence p; umi_integration_presence_init(&p);
    CHECK(umi_integration_presence_heartbeat(&p,"a",100U,50U)==UMI_STATUS_OK);
    CHECK(umi_integration_presence_is_active(&p,"a",150U));
    CHECK(!umi_integration_presence_is_active(&p,"a",151U));
    return 0;
}
