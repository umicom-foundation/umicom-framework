/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_catalogue.c
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

#include "umicom/integration/catalogue.h"

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

int main(void) {
    CHECK(umi_integration_catalogue_count()>=6U);
    CHECK(umi_integration_catalogue_at(0U)!=NULL);
    CHECK(umi_integration_catalogue_at(999U)==NULL);
    return 0;
}
