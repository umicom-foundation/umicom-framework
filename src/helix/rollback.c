/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/rollback.c
 *
 * PURPOSE:
 *   Describe a requested rollback from one generation to another.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Rollback is modeled explicitly so recovery cannot silently choose an arbitrary previous artifact.
 */

#include "umicom/helix/rollback.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise helix rollback from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_rollback_init(UmiHelixRollback *value)
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
