/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/record_history_page.c
 *
 * PURPOSE:
 *   Publish a complete bounded page only after its read transaction succeeds.
 *   Pages deliberately share the existing history codec and transaction owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Publish a complete bounded page only after its read transaction succeeds.
 * Pages deliberately share the existing history codec and transaction owner. */
#include "umicom/data/record_history_page.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus abort_page(UmiDataServer *server, UmiStatus status)
{
    return umi_data_server_rollback(server) == UMI_STATUS_OK ? status : UMI_STATUS_IO_ERROR;
}
UmiStatus umi_record_history_page(UmiDataServer *server, const char *entity_id,
    uint64_t expected_head, uint64_t before_revision, size_t limit,
    UmiRecordHistoryPage *output)
{
    if (server == NULL || entity_id == NULL || output == NULL || limit == 0U ||
        limit > UMI_RECORD_HISTORY_PAGE_CAPACITY || (before_revision != 0U && expected_head == 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Validate and capture identity before any output publication, including
     * callers reusing the entity string from their previous output page. */
    size_t length = 0U;
    while (length < 64U && entity_id[length] != '\0') {
        unsigned char byte = (unsigned char)entity_id[length];
        if (byte < 32U || byte == 127U) return UMI_STATUS_INVALID_ARGUMENT;
        ++length;
    }
    if (length == 0U || length == 64U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiRecordHistoryPage *candidate = calloc(1U, sizeof *candidate);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(candidate->entity_id, entity_id, length);
    UmiStatus status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) { free(candidate); return status; }
    UmiRecordRevision boundary;
    status = umi_record_history_latest(server, candidate->entity_id, &boundary);
    if (status == UMI_STATUS_OK) {
        candidate->head_revision = boundary.revision;
        if ((expected_head != 0U && expected_head != boundary.revision) || before_revision > boundary.revision)
            status = UMI_STATUS_INVALID_STATE;
    }
    uint64_t revision = 0U, newer_time = 0U;
    if (status == UMI_STATUS_OK) {
        if (before_revision != 0U) {
            status = umi_record_history_load(server, candidate->entity_id, before_revision, &boundary);
            revision = before_revision - 1U;
        } else revision = candidate->head_revision;
        newer_time = boundary.amendment.recorded_at_nanoseconds;
    }
    while (status == UMI_STATUS_OK && revision != 0U && candidate->count < limit) {
        UmiRecordRevision row;
        status = umi_record_history_load(server, candidate->entity_id, revision, &row);
        if (status == UMI_STATUS_OK && row.amendment.recorded_at_nanoseconds > newer_time)
            status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) {
            candidate->rows[candidate->count++] = row;
            newer_time = row.amendment.recorded_at_nanoseconds;
            --revision;
        }
    }
    if (status == UMI_STATUS_OK) {
        if (revision != 0U) candidate->next_before_revision = candidate->rows[candidate->count - 1U].revision;
        status = umi_data_server_commit(server);
    }
    if (status != UMI_STATUS_OK) status = abort_page(server, status);
    else *output = *candidate;
    free(candidate);
    return status;
}
