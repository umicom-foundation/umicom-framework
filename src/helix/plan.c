/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/plan.c
 *
 * PURPOSE:
 *   Describe one bounded change plan and its intended validation gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Plans make intended scope visible before an agent edits files, reducing uncontrolled changes outside the approved boundary.
 */

#include "umicom/helix/plan.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_helix_plan_init(UmiHelixPlan *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
