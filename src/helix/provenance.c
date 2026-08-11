/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/provenance.c
 *
 * PURPOSE:
 *   Track the source revision, provider and agent responsible for a Helix candidate.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Provenance lets reviewers reconstruct which source, model and agent produced a candidate without relying on conversational memory.
 */

#include "umicom/helix/provenance.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_helix_provenance_init(UmiHelixProvenance *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
