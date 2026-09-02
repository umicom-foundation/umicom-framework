/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/rollback_history.h
 *
 * PURPOSE:
 *   Retain a bounded history of approved rollbacks for audit and diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Rollback history helps operators understand when a generation was reversed and which target generation became active.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_ROLLBACK_HISTORY_H
#define INCLUDE_UMICOM_DELIVERY_ROLLBACK_HISTORY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/rollback.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the rollback history data shared with callers of this public contract.
 */
typedef struct UmiRollbackHistory {
    UmiRollbackPlan items[UMI_DELIVERY_MAX_RELEASES];
    size_t count;
} UmiRollbackHistory;
/**
 * Initialise rollback history from caller-provided values so later operations receive a
 * known state.
 */
void umi_rollback_history_init(UmiRollbackHistory *history);
/**
 * Add rollback history only after its inputs and available capacity have been checked.
 */
UmiStatus umi_rollback_history_add(UmiRollbackHistory *history,
                                   const UmiRollbackPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
