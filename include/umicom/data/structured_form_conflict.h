/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Bind a typed conflict review to a selected record and its exact local bytes. */
#ifndef UMICOM_DATA_STRUCTURED_FORM_CONFLICT_H
#define UMICOM_DATA_STRUCTURED_FORM_CONFLICT_H
#include "umicom/application/experience/structured_form_merge.h"
#include "umicom/data/structured_form_session.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormConflict UmiStructuredFormConflict;
/* The caller obtains incoming from a checked record read. Preparation verifies
 * metadata/content integrity, entity identity and schema before copying it.
 * An existing saved baseline is required; a new-key collision is not treated
 * as shared ancestry. Pending saves are BUSY. No storage access occurs here.
 * Check the captured baseline with record_draft_snapshot_check_store before
 * preparing against a chosen store; higher revision alone does not prove ancestry.
 * The originating session and its immutable schema must outlive the review. */
UmiStatus umi_structured_form_session_review(const UmiStructuredFormSession *session,
                                             uint64_t expected_generation,
                                             const UmiRecordDocument *incoming,
                                             UmiStructuredFormConflict **out);
void umi_structured_form_conflict_destroy(UmiStructuredFormConflict *review);
UmiStatus umi_structured_form_conflict_state(const UmiStructuredFormConflict *review,
                                             UmiFormMergeState *out);
UmiStatus umi_structured_form_conflict_field(const UmiStructuredFormConflict *review, size_t index,
                                             UmiFormMergeField *out);
UmiStatus umi_structured_form_conflict_select(UmiStructuredFormConflict *review,
                                              uint64_t expected_revision, const char *field_id,
                                              UmiFormMergeChoice choice);
UmiStatus umi_structured_form_conflict_custom(UmiStructuredFormConflict *review,
                                              uint64_t expected_revision, const char *field_id,
                                              const UmiFormAnswer *answer);
UmiStatus umi_structured_form_conflict_preview(const UmiStructuredFormConflict *review,
                                               uint64_t expected_revision, int submitting,
                                               UmiStructuredForm *out);
/* Apply the exact reviewed choice to the still-selected session. A changed
 * local answer, baseline, input byte, schema or generation refuses everything.
 * Baseline and local content change atomically; a successful application advances
 * generation and retains storage-worker request IDs. No database write occurs.
 * A later save still checks incoming's storage revision against the current head.
 * Keep the review until the call returns, then destroy it when no longer needed. */
UmiStatus umi_structured_form_session_accept_review(UmiStructuredFormSession *session,
                                                    uint64_t expected_generation,
                                                    const UmiStructuredFormConflict *review,
                                                    uint64_t expected_review_revision,
                                                    int submitting);
#ifdef __cplusplus
}
#endif
#endif
