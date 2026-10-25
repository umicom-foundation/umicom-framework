/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_messaging/outbox_review.h
 *
 * PURPOSE:
 *   Expose bounded delivery review for financial application hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCIAL_MESSAGING_OUTBOX_REVIEW_H
#define UMICOM_FINANCIAL_MESSAGING_OUTBOX_REVIEW_H
#include "umicom/financial_messaging/outbox.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UFMP_OUTBOX_PAGE_CAPACITY 32U
typedef struct UmiUfmpOutboxSummary {
  char id[65], sender[UMI_UFMP_ID_CAPACITY], recipient[UMI_UFMP_ID_CAPACITY];
  UmiUfmpMessageType type;
  UmiUfmpOutboxState state;
  uint64_t revision, updated_ms, next_attempt_ms;
  uint32_t attempts;
} UmiUfmpOutboxSummary;
typedef struct UmiUfmpOutboxPage {
  UmiUfmpOutboxSummary items[UMI_UFMP_OUTBOX_PAGE_CAPACITY];
  size_t count;
  int has_more;
  char next_cursor[65];
} UmiUfmpOutboxPage;
/* Read-only bounded review in ascending exact record-ID order. after is NULL,
 * empty or the exclusive ID from the previous page; limit is 1..32.
 * The cursor is opaque and is not a timestamp or a storage offset.
 * Each returned row is independently checked. A page is not a transaction-wide
 * snapshot: concurrent updates may change rows, and additions before the cursor
 * require a refresh from the beginning. Always supply a row's observed revision
 * when requesting a later transition. No payload bodies appear in summaries.
 * Corrupt selected bodies/state records or reserved record-key syntax fail the page;
 * output is unchanged. Existing transactions return BUSY.
 */
UmiStatus UmiUfmpOutboxReadPage(const UmiDataServer *server, const char *after,
  size_t limit, UmiUfmpOutboxPage *out);
/* Open an explicitly selected existing SQLite Data Server read-only, format
 * one page, then close it. Does not create files, execute SQL, send or retry.
 * Output is unchanged on failure; required receives the byte count including
 * the terminator after successful reading/formatting, even if capacity is small.
 * The formatted IDs and participant labels are account data: publish only
 * after the user decides that sharing them is appropriate.
 */
UmiStatus UmiUfmpOutboxInspect(const char *database_path, const char *after,
  char *out, size_t capacity, size_t *required);
#ifdef __cplusplus
}
#endif
#endif
