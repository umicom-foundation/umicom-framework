/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/approval.c
 *
 * PURPOSE:
 *   Record a human or policy approval decision for a Helix candidate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Approval remains separate from candidate state so audit records preserve who authorised a promotion and why.
 */

#include "umicom/helix/approval.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_helix_approval_init(UmiHelixApproval *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
