/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/rollback_history.c
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

#include "umicom/delivery/rollback_history.h"
#include <string.h>
void umi_rollback_history_init(UmiRollbackHistory *history)
{
    if (history != NULL) (void)memset(history, 0, sizeof(*history));
}
UmiStatus umi_rollback_history_add(UmiRollbackHistory *history,
                                   const UmiRollbackPlan *plan)
{
    if (history == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->count >= UMI_DELIVERY_MAX_RELEASES) return UMI_STATUS_CAPACITY_EXCEEDED;
    history->items[history->count++] = *plan;
    return UMI_STATUS_OK;
}
