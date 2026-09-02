/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/plan.c
 *
 * PURPOSE:
 *   Describe one bounded change plan and its intended validation gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Plans make intended scope visible before an agent edits files, reducing uncontrolled changes outside the approved boundary.
 */

#include "umicom/helix/plan.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise helix plan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_helix_plan_init(UmiHelixPlan *value)
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
