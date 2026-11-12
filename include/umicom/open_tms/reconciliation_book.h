/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/reconciliation_book.h
 *
 * PURPOSE:
 *   Track cash/trade/settlement reconciliation matches, breaks and resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_RECONCILIATION_BOOK_H
#define UMICOM_OPEN_TMS_RECONCILIATION_BOOK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_OPEN_TMS_RECONCILIATION_CAPACITY 512U
typedef struct UmiOpenTmsReconciliationItem {
 char id[64];char sourceId[64];char targetId[64];double sourceValue;double targetValue;double tolerance;int matched;int resolved;
} UmiOpenTmsReconciliationItem;
typedef struct UmiOpenTmsReconciliationBook {
 UmiOpenTmsReconciliationItem items[UMI_OPEN_TMS_RECONCILIATION_CAPACITY];size_t count;size_t matchedCount;size_t breakCount;uint64_t revision;
} UmiOpenTmsReconciliationBook;
/* Validate bounded strings, finite observations, nonnegative tolerance and
 * boolean flags. Book admission additionally requires unique IDs, derived
 * matched flags and counters consistent with every active item. matchedCount
 * counts matches; breakCount counts unmatched AND unresolved items. */
UmiStatus umi_open_tms_reconciliation_item_validate(const UmiOpenTmsReconciliationItem *item);
UmiStatus umi_open_tms_reconciliation_book_validate(const UmiOpenTmsReconciliationBook *book);
/* Add recomputes matched, preserves a valid resolved flag and refuses duplicate
 * IDs. Resolve changes only local review state, never an external transaction.
 * Checked coverage leaves output unchanged on refusal. Empty valid books give
 * zero; legacy coverage also gives zero on refusal. Keep inputs stable and
 * output disjoint. This double-valued tolerance model is not exact accounting. */
UmiStatus umi_open_tms_reconciliation_book_coverage_checked(const UmiOpenTmsReconciliationBook *book, double *out);

void umi_open_tms_reconciliation_book_init(UmiOpenTmsReconciliationBook *book);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_open_tms_reconciliation_book_add(UmiOpenTmsReconciliationBook *book,const UmiOpenTmsReconciliationItem *item);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_open_tms_reconciliation_book_resolve(UmiOpenTmsReconciliationBook *book,const char *id);
double umi_open_tms_reconciliation_book_coverage(const UmiOpenTmsReconciliationBook *book);
#ifdef __cplusplus
}
#endif
#endif
