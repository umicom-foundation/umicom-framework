/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bounded history pages let application hosts inspect amendments without
 * scanning an entire store or implementing private persistence-key rules. */
#ifndef UMICOM_DATA_RECORD_HISTORY_PAGE_H
#define UMICOM_DATA_RECORD_HISTORY_PAGE_H
#include "umicom/data/record_history.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RECORD_HISTORY_PAGE_CAPACITY 16U

typedef struct UmiRecordHistoryPage {
    char entity_id[64];
    uint64_t head_revision;
    uint64_t next_before_revision;
    size_t count;
    UmiRecordRevision rows[UMI_RECORD_HISTORY_PAGE_CAPACITY];
} UmiRecordHistoryPage;

/* Rows are newest first. Start with expected_head=0 and before_revision=0.
 * Continue using the returned head_revision and next_before_revision; zero
 * next_before_revision means no older page. Continuations require a nonzero
 * expected_head. A changed head returns INVALID_STATE: refresh from the start.
 * limit must be 1..16. before_revision is exclusive and cannot exceed the head.
 * A cursor of one returns an empty page, because revision zero is not a row.
 *
 * The query owns one Data Server transaction. It checks identity, revision
 * encoding and timestamp order within the page and at its preceding boundary.
 * This is a metadata view, not payload verification, full-chain verification,
 * authentication or proof against malicious rewriting. Use document_load to
 * inspect a selected payload and document_verify for complete content checks.
 *
 * Outputs are unchanged on every failure, including transaction completion.
 * Existing transactions return BUSY. No callbacks, UI access or worker creation
 * occur; hosts dispatch the query on their storage worker and own the result. */
UmiStatus umi_record_history_page(UmiDataServer *server, const char *entity_id,
    uint64_t expected_head, uint64_t before_revision, size_t limit,
    UmiRecordHistoryPage *output);
#ifdef __cplusplus
}
#endif
#endif
