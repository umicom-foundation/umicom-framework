/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/experiment.c
 *
 * PURPOSE:
 *   Record one controlled Helix experiment and comparison result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Experiments compare candidates against a known baseline instead of treating every generated change as an improvement.
 */

#include "umicom/helix/experiment.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise helix experiment from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_experiment_init(UmiHelixExperiment *value)
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
