/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_acceptance_report.c
 *
 * PURPOSE:
 *   Implement the test acceptance report behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | acceptance_report | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/test_runtime/check.h"
#include <stdlib.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionRuntime *runtime = calloc(1U, sizeof(*runtime));
    UMI_TEST_REQUIRE(runtime != NULL);
    UMI_TEST_REQUIRE(umi_application_production_runtime_init(&adoption, umi_test_application_production_all_available, NULL, runtime) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(runtime->acceptance.accepted);
    UMI_TEST_REQUIRE(runtime->acceptance.requirement_count == runtime->acceptance.accepted_count);
    free(runtime);
    return 0;
}

