/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/rollback_history.h
 *
 * PURPOSE:
 *   Retain a bounded history of approved rollbacks for audit and diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiRollbackHistory {
    UmiRollbackPlan items[UMI_DELIVERY_MAX_RELEASES];
    size_t count;
} UmiRollbackHistory;
void umi_rollback_history_init(UmiRollbackHistory *history);
UmiStatus umi_rollback_history_add(UmiRollbackHistory *history,
                                   const UmiRollbackPlan *plan);

#ifdef __cplusplus
}
#endif

#endif
