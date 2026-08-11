/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/rollback.c
 *
 * PURPOSE:
 *   Describe a requested rollback from one generation to another.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Rollback is modeled explicitly so recovery cannot silently choose an arbitrary previous artifact.
 */

#include "umicom/helix/rollback.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_helix_rollback_init(UmiHelixRollback *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
