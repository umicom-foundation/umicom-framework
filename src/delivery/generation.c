/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/generation.c
 *
 * PURPOSE:
 *   Track immutable installed generations and their release identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A generation is the deployable unit that supports safe promotion and rollback without mutating the previous release in place.
 */

#include "umicom/delivery/generation.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_generation_init(UmiGeneration *generation,
                              uint64_t number,
                              const char *release_id,
                              const char *root)
{
    if (generation == NULL || release_id == NULL || root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(generation, 0, sizeof(*generation));
    generation->number = number;
    generation->health = UMI_EVIDENCE_UNKNOWN;
    if (umi_delivery_copy_text(generation->release_id,
                               sizeof(generation->release_id),
                               release_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(generation->root,
                               sizeof(generation->root),
                               root) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

void umi_generation_mark_active(UmiGeneration *generation, int active)
{
    if (generation != NULL) generation->active = active != 0;
}
