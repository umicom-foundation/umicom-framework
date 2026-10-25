/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/data_history/record_documents.c
 *
 * PURPOSE:
 *   Two non-production consumers share content storage instead of copying SQL,
 *   digest calculation and stale-write handling into application modules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Two non-production consumers share content storage instead of copying SQL,
 * digest calculation and stale-write handling into application modules. */
#include "umicom/data/record_document.h"
#include <stdio.h>
#include <string.h>

static int save_example(UmiDataServer *server, const char *entity,
                         const char *original, const char *amended)
{
    UmiRecordAmendment metadata = {.source = "synthetic-example",
        .actor = "fictional-reviewer", .reason = "Record an example",
        .recorded_at_nanoseconds = 100U};
    (void)snprintf(metadata.entity_id, sizeof metadata.entity_id, "%s", entity);
    UmiRecordRevision revision;
    if (umi_record_document_append(server, &metadata, original, strlen(original),
                                   0U, &revision) != UMI_STATUS_OK) return 1;
    uint64_t reviewed = revision.revision;
    strcpy(metadata.reason, "Amend the example after review");
    if (umi_record_document_append(server, &metadata, amended, strlen(amended),
                                   reviewed, &revision) != UMI_STATUS_OK) return 1;
    /* An older window must reload before proposing another amendment. */
    if (umi_record_document_append(server, &metadata, "stale", 5U,
                                   reviewed, &revision) != UMI_STATUS_INVALID_STATE) return 1;
    UmiRecordDocument document;
    if (umi_record_document_load(server, entity, 1U, &document) != UMI_STATUS_OK ||
        document.length != strlen(original) ||
        memcmp(document.content, original, document.length) != 0) return 1;
    uint64_t count = 0U;
    if (umi_record_document_verify(server, entity, &count) != UMI_STATUS_OK) return 1;
    printf("%s: %llu intact revisions; original retained; stale edit refused\n",
        entity, (unsigned long long)count);
    return count == 2U ? 0 : 1;
}
int main(void)
{
    UmiDataServer *server = NULL;
    if (umi_data_server_create_memory(&server) != UMI_STATUS_OK) return 1;
    int result = save_example(server, "synthetic-care-note",
        "Fictional administrative follow-up: contact pending.",
        "Fictional administrative follow-up: contact completed.");
    if (result == 0) result = save_example(server, "synthetic-bank-review",
        "Fictional statement row: reference not reviewed.",
        "Fictional statement row: reference reviewed; no payment initiated.");
    umi_data_server_destroy(server);
    return result;
}
