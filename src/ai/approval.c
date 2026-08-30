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

/* BEGINNER NOTE:
 * Approval is a first-class record so tool execution and remote calls can be governed and audited.
 */

#include "umicom/ai/approval.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_ai_approval_init(UmiAiApproval *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
