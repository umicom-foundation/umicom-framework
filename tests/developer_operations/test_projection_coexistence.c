/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_operations/test_projection_coexistence.c
 *
 * PURPOSE:
 *   Compile both developer service families together and compare facade results with the existing catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/operation.h"
#include "umicom/developer_operations/projection.h"
#include <stdio.h>
#include <string.h>

UmiStatus UmiTestLegacyProjection(const char *, const UmiDeveloperOperationProjectionInput *, UmiDeveloperOperationProjectionSnapshot *);
int UmiTestProjectionReverseIncludeOrder(void);
#define REQUIRE(test) do { if (!(test)) { fprintf(stderr, "Projection test failed: %s\n", #test); return 1; } } while (0)

int main(void)
{
    const char *ids[] = {"coverage.test-result-summary", "debug.session", "vcs.status"};
    UmiDeveloperOperationSnapshot executable = {0};
    UmiDeveloperOperationProjectionInput input;
    UmiDeveloperOperationProjectionSnapshot actual, expected;
    executable.api_version = UMI_DEVELOPER_OPERATION_API_VERSION;
    executable.argument_count = 1U;
    REQUIRE(sizeof(executable.id) == UMI_DEVELOPER_ID_CAPACITY);
    REQUIRE(sizeof(executable.arguments) == UMI_DEVELOPER_MAX_ARGUMENTS * UMI_DEVELOPER_ARGUMENT_CAPACITY);
    REQUIRE(UmiTestProjectionReverseIncludeOrder() == 0);
    for (size_t index = 0U; index < 3U; ++index) {
        for (unsigned variant = 0U; variant < 4U; ++variant) {
            UmiDeveloperOperationProjectionInputInit(&input);
            input.label = "Umicom workbench"; input.primary = 7U; input.secondary = 2U;
            input.total = 11U; input.baseline = 30.0; input.threshold = 80.0;
            input.active = (int)(variant & 1U); input.blocked = (int)((variant >> 1U) & 1U); input.revision = 91U;
            REQUIRE(UmiTestLegacyProjection(ids[index], &input, &expected) == UMI_STATUS_OK);
            REQUIRE(UmiDeveloperOperationsProjectionEvaluate(ids[index], &input, &actual) == UMI_STATUS_OK);
            REQUIRE(strcmp(actual.id, expected.id) == 0 && strcmp(actual.label, expected.label) == 0);
            REQUIRE(actual.primary == expected.primary && actual.secondary == expected.secondary && actual.total == expected.total);
            REQUIRE(actual.missed == expected.missed && actual.percent == expected.percent && actual.delta == expected.delta);
            REQUIRE(actual.active == expected.active && actual.blocked == expected.blocked && actual.ready == expected.ready);
            REQUIRE(actual.attention == expected.attention && actual.revision == expected.revision);
        }
    }
    memset(&actual, 1, sizeof(actual));
    REQUIRE(UmiDeveloperOperationsProjectionEvaluate("missing.projection", &input, &actual) == UMI_STATUS_NOT_FOUND);
    REQUIRE(actual.id[0] == '\0' && actual.primary == 0U && actual.percent == 0.0);
    REQUIRE(UmiDeveloperOperationsProjectionEvaluate("vcs.status", NULL, &actual) == UMI_STATUS_INVALID_ARGUMENT);
    puts("Developer operation and projection contracts coexist; catalogue results preserved.");
    return 0;
}
