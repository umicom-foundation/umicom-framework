/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/fx_forward_review.h
 *
 * PURPOSE:
 *   Bind treasury transition previews to complete active book state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OPEN_TMS_FX_FORWARD_REVIEW_H
#define UMICOM_OPEN_TMS_FX_FORWARD_REVIEW_H

#include "umicom/open_tms/fx_forward_book.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Owned immutable preview of one local lifecycle transition. It copies all
 * active book records, the owner revision and the requested next trade version.
 * No caller storage is retained. It can outlive the source book. This is review
 * evidence, not an approval credential, reservation, trade execution or payment.
 * Equivalent in-memory books may consume the review. Serialise all access. */
typedef struct UmiOpenTmsFxForwardReview UmiOpenTmsFxForwardReview;
typedef struct UmiOpenTmsFxForwardReviewSnapshot {
    uint64_t beforeRevision;
    uint64_t afterRevision;
    size_t recordCount;
    UmiOpenTmsFxForward before;
    UmiOpenTmsFxForward after;
} UmiOpenTmsFxForwardReviewSnapshot;

/* Run the existing transition on a disposable copy. Failure clears *outReview;
 * the source is unchanged. A malformed or exhausted book cannot produce a
 * review. Unknown trade IDs return NOT_FOUND. Destroy accepts NULL. */
UmiStatus umi_open_tms_fx_forward_review_create(
    const UmiOpenTmsFxForwardBook *book, const char *tradeId,
    UmiOpenTmsFxForwardState nextState, uint64_t nextVersion,
    UmiOpenTmsFxForwardReview **outReview);
void umi_open_tms_fx_forward_review_destroy(UmiOpenTmsFxForwardReview *review);

/* Copy values for display. Mutating this snapshot never changes the review.
 * Failure leaves *outSnapshot unchanged. No result is an execution receipt. */
UmiStatus umi_open_tms_fx_forward_review_read(
    const UmiOpenTmsFxForwardReview *review,
    UmiOpenTmsFxForwardReviewSnapshot *outSnapshot);

/* Apply only the stored transition after comparing every active record and the
 * complete owner revision/count. Strings compare their bounded logical values,
 * finite numbers their values, and all other fields exactly; padding and unused
 * capacity are not identity. A changed book returns BUSY without mutation.
 * A successful apply changes the revision, so replay on that book is BUSY.
 * Refresh external state, enforce current actor permissions and decide whether
 * to proceed in the hosting service. This API has no storage or network effects. */
UmiStatus umi_open_tms_fx_forward_review_apply(
    UmiOpenTmsFxForwardBook *book, const UmiOpenTmsFxForwardReview *review);

#ifdef __cplusplus
}
#endif
#endif
