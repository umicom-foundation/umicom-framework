/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/lifecycle_event.h
 *
 * PURPOSE:
 *   Describe reusable fixing, payment, exercise, maturity and termination events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_LIFECYCLE_EVENT_H
#define UMICOM_FINANCE_CORE_LIFECYCLE_EVENT_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the lifecycle event data shared with callers of this public contract.
 */
typedef struct UmiLifecycleEvent { UmiFinancialId event_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; UmiFinancialDate effective_date; uint32_t state; bool active; } UmiLifecycleEvent;
/* Initialize the typed financial record. */ UmiStatus umi_lifecycle_event_init(UmiLifecycleEvent *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state);
/* Validate the typed financial record. */ bool umi_lifecycle_event_is_valid(const UmiLifecycleEvent *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_lifecycle_event_archive_encode(const UmiLifecycleEvent *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_lifecycle_event_archive_decode(const void *bytes, size_t byte_count,
    UmiLifecycleEvent *value);

#ifdef __cplusplus
}
#endif

#endif
