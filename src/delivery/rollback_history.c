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
/*
 * Initialise rollback history from caller-provided values so later operations receive a
 * known state.
 */
void umi_rollback_history_init(UmiRollbackHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history != NULL) (void)memset(history, 0, sizeof(*history));
}
/* Add rollback history only after its inputs and available capacity have been checked. */
UmiStatus umi_rollback_history_add(UmiRollbackHistory *history,
                                   const UmiRollbackPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count >= UMI_DELIVERY_MAX_RELEASES) return UMI_STATUS_CAPACITY_EXCEEDED;
    history->items[history->count++] = *plan;
    return UMI_STATUS_OK;
}
