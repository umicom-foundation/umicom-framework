/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/generation.c
 *
 * PURPOSE:
 *   Describe an immutable promoted software generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A generation identifies the exact artifact and checksum that was promoted, giving operations a stable rollback target.
 */

#include "umicom/helix/generation.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise helix generation from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_generation_init(UmiHelixGeneration *value)
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
