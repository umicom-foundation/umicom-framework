/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/structured_form_bundle.h
 *
 * PURPOSE:
 *   Review an entire typed history before an explicit, non-overwriting restore.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Review an entire typed history before an explicit, non-overwriting restore. */
#ifndef UMICOM_DATA_STRUCTURED_FORM_BUNDLE_H
#define UMICOM_DATA_STRUCTURED_FORM_BUNDLE_H
#include "umicom/data/record_bundle.h"
#include "umicom/data/record_transfer.h"
#include "umicom/data/structured_form_reader.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormBundleReview UmiStructuredFormBundleReview;
typedef int (*UmiStructuredFormBundleKeyCheck)(const char *entity_id, void *context);
typedef struct UmiStructuredFormBundleSummary {
  UmiRecordRevision first;
  UmiRecordRevision latest;
  char form_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  char bundle_sha256[65];
  size_t revision_count;
  size_t content_bytes;
  uint64_t first_schema_version;
  uint64_t latest_schema_version;
} UmiStructuredFormBundleSummary;
/* Copy the immutable bundle and validate EVERY revision against an exact trusted
 * schema of expected_form_id. The nonempty namespace must end in a dot; the key
 * must contain a suffix. An optional pure check applies product key policy.
 * The check and context are used only during this call. Catalogue/schemas are
 * borrowed immutable data that must outlive this review and inspected results.
 * Drafts with unanswered required fields are retained as drafts, not submitted.
 * No database or file access occurs; outputs are unchanged on failure. */
UmiStatus umi_structured_form_bundle_review(const UmiRecordBundle *bundle,
                                            const UmiStructuredFormCatalogue *catalogue,
                                            const char *expected_form_id,
                                            const char *entity_namespace,
                                            UmiStructuredFormBundleKeyCheck key_check,
                                            void *context, UmiStructuredFormBundleReview **out);
/* Clone captured history for an independent worker lifetime. The catalogue is
 * still borrowed and immutable; no callbacks or storage operations are repeated. */
UmiStatus umi_structured_form_bundle_review_clone(const UmiStructuredFormBundleReview *review,
                                                  UmiStructuredFormBundleReview **out);
void umi_structured_form_bundle_review_destroy(UmiStructuredFormBundleReview *review);
UmiStatus umi_structured_form_bundle_summary(const UmiStructuredFormBundleReview *review,
                                             UmiStructuredFormBundleSummary *out);
/* Zero-based ascending history; returns an owned form with a borrowed schema.
 * Allocate this large result on the heap. Inspection never edits the plan. */
UmiStatus umi_structured_form_bundle_inspect(const UmiStructuredFormBundleReview *review,
                                             size_t index, UmiStructuredFormHistorical *out);
/* The supplied digest must match the summary the caller actually reviewed.
 * The application must separately authorize the selected destination and action.
 * Restore preserves original authorship and timestamps; they are NOT verified
 * identities. Existing or racing records are refused by the shared transaction.
 * Caller owns server serialization; no nested transaction is started. Failure
 * retains this review for inspection/retry, and leaves output unchanged. */
UmiStatus umi_structured_form_bundle_restore(UmiDataServer *server,
                                             const UmiStructuredFormBundleReview *review,
                                             const char *reviewed_sha256, UmiRecordRevision *out);
/* Preserve the typed review checks and atomically add separate local import
 * provenance. The original plain restore API above retains its behaviour. */
UmiStatus umi_structured_form_bundle_restore_receipted(UmiDataServer *server,
                                                       const UmiStructuredFormBundleReview *review,
                                                       const char *reviewed_sha256,
                                                       const UmiRecordTransferRequest *request,
                                                       UmiRecordTransferReceipt *out);
#ifdef __cplusplus
}
#endif
#endif
