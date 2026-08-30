/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/candidate.c
 *
 * PURPOSE:
 *   Represent an isolated Helix software candidate and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Candidates are isolated workspaces rather than in-place production mutations, which makes validation and rollback practical.
 */

#include "umicom/helix/candidate.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_helix_candidate_init(UmiHelixCandidate *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
