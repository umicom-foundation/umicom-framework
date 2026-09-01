/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/observation.c
 *
 * PURPOSE:
 *   Describe an observed product or runtime condition before diagnosis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Observations are facts, not proposed fixes; keeping them separate prevents diagnostic guesses from overwriting raw evidence.
 */

#include "umicom/helix/observation.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_helix_observation_init(UmiHelixObservation *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
