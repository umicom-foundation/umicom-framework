/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data_history/record_amendments.c
 *
 * PURPOSE:
 *   Two fictional workflows share the same amendment service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Two fictional workflows share the same amendment service. */
#include "umicom/data/record_history.h"
#include <stdio.h>
#include <string.h>

static UmiStatus demonstrate(UmiDataServer *server, const char *entity)
{
    UmiRecordAmendment amendment = {.source = "example-import", .actor = "example-reviewer",
        .reason = "Import an empty example document", .observed_at_nanoseconds = 1U,
        .recorded_at_nanoseconds = 2U};
    (void)snprintf(amendment.entity_id, sizeof amendment.entity_id, "%s", entity);
    strcpy(amendment.content_sha256, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    UmiRecordRevision saved;
    UmiStatus status = umi_record_history_append(server, &amendment, 0U, &saved);
    if (status != UMI_STATUS_OK) { return status; }
    strcpy(amendment.reason, "Correct the source label after review");
    strcpy(amendment.source, "reviewed-example-import");
    amendment.recorded_at_nanoseconds = 3U;
    status = umi_record_history_append(server, &amendment, saved.revision, &saved);
    if (status != UMI_STATUS_OK) { return status; }
    UmiRecordRevision original;
    status = umi_record_history_load(server, entity, 1U, &original);
    if (status != UMI_STATUS_OK) { return status; }
    printf("%s: revision %llu follows %llu; original source remains %s\n", entity,
        (unsigned long long)saved.revision, (unsigned long long)saved.previous_revision,
        original.amendment.source);
    return UMI_STATUS_OK;
}
int main(void)
{
    UmiDataServer *server = NULL;
    UmiStatus status = umi_data_server_create_memory(&server);
    if (status == UMI_STATUS_OK) {
        status = demonstrate(server, "synthetic-care-observation");
    }
    if (status == UMI_STATUS_OK) {
        status = demonstrate(server, "synthetic-bank-import");
    }
    umi_data_server_destroy(server);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Amendment example failed: %s\n", umi_status_text(status));
        return 1;
    }
    return 0;
}
