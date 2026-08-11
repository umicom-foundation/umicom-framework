/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/experiment.c
 *
 * PURPOSE:
 *   Record one controlled Helix experiment and comparison result.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Experiments compare candidates against a known baseline instead of treating every generated change as an improvement.
 */

#include "umicom/helix/experiment.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_helix_experiment_init(UmiHelixExperiment *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
