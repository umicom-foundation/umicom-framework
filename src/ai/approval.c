/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/approval.c
 *
 * PURPOSE:
 *   Represent a human or policy approval decision for sensitive AI actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Approval is a first-class record so tool execution and remote calls can be governed and audited.
 */

#include "umicom/ai/approval.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai approval from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_approval_init(UmiAiApproval *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
