/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Installed data-SDK example: page amendment metadata without a GUI dependency. */
#include "umicom/data/record_history_page.h"
#include <inttypes.h>
#include <stdio.h>
int main(void)
{
    UmiDataServer *server = NULL;
    UmiStatus status = umi_data_server_create_memory(&server);
    if (status != UMI_STATUS_OK) return 1;
    UmiRecordAmendment metadata = {.entity_id = "fictional-review", .source = "SDK example",
        .actor = "Example reviewer", .reason = "Synthetic amendment",
        .content_sha256 = "0000000000000000000000000000000000000000000000000000000000000000"};
    UmiRecordRevision row;
    for (uint64_t i = 0U; i < 19U && status == UMI_STATUS_OK; ++i) {
        metadata.recorded_at_nanoseconds = i + 1U;
        status = umi_record_history_append(server, &metadata, i, &row);
    }
    UmiRecordHistoryPage page;
    uint64_t head = 0U, before = 0U;
    size_t pages = 0U, rows = 0U;
    while (status == UMI_STATUS_OK) {
        status = umi_record_history_page(server, metadata.entity_id, head, before, 16U, &page);
        if (status != UMI_STATUS_OK) break;
        ++pages; rows += page.count;
        head = page.head_revision; before = page.next_before_revision;
        if (before == 0U) break;
    }
    if (status == UMI_STATUS_OK && (head != 19U || pages != 2U || rows != 19U)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) printf("History head %" PRIu64 ": %zu revisions across %zu pages\n", head, rows, pages);
    else fprintf(stderr, "History example failed: %s\n", umi_status_text(status));
    umi_data_server_destroy(server);
    return status == UMI_STATUS_OK ? 0 : 1;
}
