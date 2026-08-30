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
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiLifecycleEvent { UmiFinancialId event_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; UmiFinancialDate effective_date; uint32_t state; bool active; } UmiLifecycleEvent;
/* Initialize the typed financial record. */ UmiStatus umi_lifecycle_event_init(UmiLifecycleEvent *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state);
/* Validate the typed financial record. */ bool umi_lifecycle_event_is_valid(const UmiLifecycleEvent *item);
#ifdef __cplusplus
}
#endif

#endif
