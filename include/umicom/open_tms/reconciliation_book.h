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
void umi_open_tms_reconciliation_book_init(UmiOpenTmsReconciliationBook *book);
UmiStatus umi_open_tms_reconciliation_book_add(UmiOpenTmsReconciliationBook *book,const UmiOpenTmsReconciliationItem *item);
UmiStatus umi_open_tms_reconciliation_book_resolve(UmiOpenTmsReconciliationBook *book,const char *id);
double umi_open_tms_reconciliation_book_coverage(const UmiOpenTmsReconciliationBook *book);
#ifdef __cplusplus
}
#endif
#endif
