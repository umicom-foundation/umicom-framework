/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_artifacts.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "value_archive/construction_cases.h"
#include "umicom/delivery/artifact_set.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted artifact value. */
static int UmiDeliveryArtifactConstructionCases(void)
{
    UmiDeliveryArtifact *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_delivery_artifact_init(value, "studio.exe", UMI_ARTIFACT_EXECUTABLE, "bin/studio.exe") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->artifact_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_artifact_init(value, oversized, UMI_ARTIFACT_EXECUTABLE, "bin/studio.exe") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_artifact_init(value, value->artifact_id, UMI_ARTIFACT_EXECUTABLE, "bin/studio.exe") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->artifact_id, "studio.exe") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->path) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_artifact_init(value, "studio.exe", UMI_ARTIFACT_EXECUTABLE, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_delivery_artifact_init(value, "studio.exe", UMI_ARTIFACT_EXECUTABLE, value->path) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->path, "bin/studio.exe") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDeliveryArtifactConstructionCases() != 0) return 1;

    UmiArtifactSet set;
    UmiDeliveryArtifact a;
    umi_artifact_set_init(&set);
    assert(umi_delivery_artifact_init(&a, "studio.exe", UMI_ARTIFACT_EXECUTABLE, "bin/studio.exe") == UMI_STATUS_OK);
    a.size_bytes = 42U;
    assert(umi_artifact_set_add(&set, &a) == UMI_STATUS_OK);
    assert(umi_artifact_set_find(&set, "studio.exe") != 0);
    assert(umi_artifact_set_total_size(&set) == 42U);
    return 0;
}
