/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/evidence.c
 *
 * PURPOSE:
 *   Record one immutable piece of evidence used by a Helix decision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Evidence is recorded before planning so candidate changes can always be traced back to the observations that justified them.
 */

#include "umicom/helix/evidence.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_helix_evidence_init(UmiHelixEvidence *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
