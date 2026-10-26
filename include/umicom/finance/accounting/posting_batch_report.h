/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/posting_batch_report.h
 *
 * PURPOSE:
 *   Export captured journal review amounts as an independently owned exact CSV report.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_POSTING_BATCH_REPORT_H
#define UMICOM_FINANCE_ACCOUNTING_POSTING_BATCH_REPORT_H
#include "umicom/base/csv_document.h"
#include "umicom/finance/accounting/posting_batch.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Export one captured eligibility review, in journal/line order, followed by
 * one total row per journal. Raw minor units and fixed-scale decimal text are
 * both exact signed-64-bit values; no floating-point conversion, rounding,
 * currency conversion or cross-journal total occurs.
 *
 * currency (three uppercase letters) and scale (0..9) are explicit caller
 * assumptions applying to every included account. Journal entries do not
 * contain currency metadata: this API cannot verify those assumptions or
 * establish account-currency agreement. Mixed-currency batches require separate
 * reviews. A valid code is a syntax check, not a currency registry lookup.
 *
 * Every row is marked "captured-eligibility". The report never asserts current
 * readiness, approval, durable posting or settlement. It continues to describe
 * the same captured inputs after live edits or Apply. Keep any authoritative
 * posting receipt separate from this review report.
 *
 * max_bytes includes the terminating zero and must be 1..UMI_CSV_MAX_BYTES.
 * Output storage must be separate from inputs. *out_report is NULL on failure;
 * no partial report escapes. Text uses the shared CSV UTF-8 and formula-text
 * protection policy. The caller owns the returned document, independent of
 * review lifetime, and destroys it with UmiCsvDocumentDestroy. No file or
 * provider I/O occurs. Call on the review owner's serialized thread. */
UmiStatus umi_accounting_posting_batch_report(const UmiAccountingPostingBatchReview *review,
                                              const UmiCurrency *currency, uint8_t scale,
                                              size_t max_bytes, UmiCsvDocument **out_report);

#ifdef __cplusplus
}
#endif
#endif
