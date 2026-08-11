/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_application.c
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

#include "umicom/integration/application.h"

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

int main(void) {
    UmiIntegrationApplication a;
    umi_integration_application_init(&a);
    CHECK(umi_integration_application_set_identity(&a,"umicom.test","Test")==UMI_STATUS_OK);
    CHECK(umi_integration_application_add_capability(&a,"cap.alpha/1")==UMI_STATUS_OK);
    CHECK(umi_integration_application_has_capability(&a,"cap.alpha/1"));
    return 0;
}
