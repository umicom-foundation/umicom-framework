/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/provenance.c
 *
 * PURPOSE:
 *   Track the source revision, provider and agent responsible for a Helix candidate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Provenance lets reviewers reconstruct which source, model and agent produced a candidate without relying on conversational memory.
 */

#include "umicom/helix/provenance.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise helix provenance from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_provenance_init(UmiHelixProvenance *value)
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
