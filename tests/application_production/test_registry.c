/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_registry.c
 *
 * PURPOSE:
 *   Implement the test registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | registry | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionRegistry registry;
    umi_application_production_registry_init(&registry);
    assert(umi_application_production_registry_register(&registry, &adoption) == UMI_STATUS_OK);
    assert(umi_application_production_registry_register(&registry, &adoption) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_application_production_registry_find(&registry, "org.umicom.studio") != NULL);
    return 0;
}

