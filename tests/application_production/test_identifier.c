/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_identifier.c
 *
 * PURPOSE:
 *   Implement the test identifier behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | identifier | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/application/production/identifier.h"
int main(void) {
    UmiApplicationProductionIdentifier a = {{0}}, b = {{0}};
    assert(umi_application_production_identifier_set(&a, "org.umicom.studio") == UMI_STATUS_OK);
    assert(umi_application_production_identifier_set(&b, "org.umicom.studio") == UMI_STATUS_OK);
    assert(umi_application_production_identifier_valid(&a));
    assert(umi_application_production_identifier_equal(&a, &b));
    return 0;
}

