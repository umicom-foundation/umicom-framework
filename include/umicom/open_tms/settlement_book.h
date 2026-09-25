/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/open_tms/settlement_book.h
 *
 * PURPOSE:
 *   Track treasury settlement obligations, matching, repair and completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_SETTLEMENT_BOOK_H
#define UMICOM_OPEN_TMS_SETTLEMENT_BOOK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_OPEN_TMS_SETTLEMENT_CAPACITY 512U

typedef enum UmiOpenTmsSettlementState {
    UMI_OPEN_TMS_SETTLEMENT_PENDING=0,
    UMI_OPEN_TMS_SETTLEMENT_MATCHED=1,
    UMI_OPEN_TMS_SETTLEMENT_SENT=2,
    UMI_OPEN_TMS_SETTLEMENT_SETTLED=3,
    UMI_OPEN_TMS_SETTLEMENT_FAILED=4,
    UMI_OPEN_TMS_SETTLEMENT_REPAIR=5
} UmiOpenTmsSettlementState;

typedef struct UmiOpenTmsSettlementRecord {
    char id[64];
    char sourceId[64];
    char accountId[64];
    char currency[8];
    double amount;
    uint64_t dueMilliseconds;
    UmiOpenTmsSettlementState state;
    uint64_t version;
} UmiOpenTmsSettlementRecord;

typedef struct UmiOpenTmsSettlementBook {
    UmiOpenTmsSettlementRecord records[UMI_OPEN_TMS_SETTLEMENT_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiOpenTmsSettlementBook;

void umi_open_tms_settlement_book_init(UmiOpenTmsSettlementBook *book);
UmiStatus umi_open_tms_settlement_book_add(
    UmiOpenTmsSettlementBook *book,const UmiOpenTmsSettlementRecord *record);
UmiStatus umi_open_tms_settlement_book_transition(
    UmiOpenTmsSettlementBook *book,const char *id,
    UmiOpenTmsSettlementState state,uint64_t version);
size_t umi_open_tms_settlement_book_failed_count(const UmiOpenTmsSettlementBook *book);
size_t umi_open_tms_settlement_book_overdue_count(
    const UmiOpenTmsSettlementBook *book,uint64_t nowMilliseconds);

#ifdef __cplusplus
}
#endif
#endif
