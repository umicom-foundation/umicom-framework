/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/mutation.c
 *
 * PURPOSE:
 *   Describe one bounded source mutation proposed by an agent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Mutations identify the exact path and whether an operation is destructive, giving policy a precise review point.
 */

#include "umicom/helix/mutation.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise helix mutation from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_mutation_init(UmiHelixMutation *value)
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
