/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/value_archive/compare.c
 *
 * PURPOSE:
 *   Demonstrate measured and completed canonical comparison of project configuration snapshots in C23.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/project/configuration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiProjectConfigurationSnapshot saved = {0}, draft;
    saved.struct_size = (uint32_t)sizeof(saved);
    saved.api_version = UMI_PROJECT_CONFIGURATION_API_VERSION;
    saved.revision = 1U;
    memcpy(saved.id, "native-debug", sizeof("native-debug"));
    memcpy(saved.build_type, "Debug", sizeof("Debug"));
    draft = saved;
    UmiValueArchiveComparison observation = {0};
    UmiStatus status = umi_project_configuration_snapshot_archive_compare_with_workspace(
        &saved, &draft, NULL, 0U, &observation);
    if (status != UMI_STATUS_OK || observation.relation != UMI_VALUE_ARCHIVE_NOT_COMPARED)
        return 1;
    size_t capacity = observation.required_workspace;
    unsigned char *scratch = malloc(capacity);
    if (scratch == NULL) return 1;
    int result = 1;

    status = umi_project_configuration_snapshot_archive_compare_with_workspace(
        &saved, &draft, scratch, capacity, &observation);
    if (status != UMI_STATUS_OK || observation.relation != UMI_VALUE_ARCHIVE_EQUAL)
        goto done;
    (void)puts("Umicom Framework: unchanged saved fields.");

    /* A same-length edit demonstrates comparison using the already-owned
     * storage. For larger edits, remeasure and grow capacity before retrying. */
    memcpy(draft.build_type, "debug", sizeof("debug"));
    status = umi_project_configuration_snapshot_archive_compare_with_workspace(
        &saved, &draft, scratch, capacity, &observation);
    if (status != UMI_STATUS_OK || observation.relation != UMI_VALUE_ARCHIVE_DIFFERENT)
        goto done;
    (void)puts("Umicom Framework: draft differs; no live state was published.");
    result = 0;
done:
    /* These profiles contain no credentials. Sensitive consumers must apply
     * their own supported secure-erasure policy before releasing scratch. */
    free(scratch);
    return result;
}
