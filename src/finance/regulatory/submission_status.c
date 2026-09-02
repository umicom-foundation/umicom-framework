/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/submission_status.c
 *
 * PURPOSE:
 *   Enforce monotonic regulatory submission lifecycle transitions.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/submission_status.h"

/*
 * Provide the reg submission status transition allowed operation used by this module and
 * its client applications.
 */
int umi_reg_submission_status_transition_allowed(UmiRegLifecycleState from, UmiRegLifecycleState to)
{
    /* Apply this operation only while the related capability or state is available. */
    if (from == UMI_REG_STATE_DRAFT) return to == UMI_REG_STATE_VALIDATED || to == UMI_REG_STATE_CANCELLED;
    /* Apply this operation only while the related capability or state is available. */
    if (from == UMI_REG_STATE_VALIDATED) return to == UMI_REG_STATE_APPROVED || to == UMI_REG_STATE_DRAFT || to == UMI_REG_STATE_CANCELLED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (from == UMI_REG_STATE_APPROVED) return to == UMI_REG_STATE_SUBMITTED || to == UMI_REG_STATE_CANCELLED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (from == UMI_REG_STATE_SUBMITTED) return to == UMI_REG_STATE_ACCEPTED || to == UMI_REG_STATE_REJECTED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (from == UMI_REG_STATE_REJECTED) return to == UMI_REG_STATE_DRAFT || to == UMI_REG_STATE_CANCELLED;
    return 0;
}
