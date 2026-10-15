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
#include "../base/value_construction_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise generation from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_generation_init(UmiGeneration *generation,
                              uint64_t number,
                              const char *release_id,
                              const char *root)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (generation == NULL || release_id == NULL || root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(generation, 0, sizeof(*generation));
    generation->number = number;
    generation->health = UMI_EVIDENCE_UNKNOWN;
    /* Use the stable identifier comparison to choose the matching record or policy. */
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
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_generation_init_build_value(UmiGeneration *generation,
                              uint64_t number,
                              const char *release_id,
                              const char *root)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (generation == NULL || release_id == NULL || root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(generation, 0, sizeof(*generation));
    generation->number = number;
    generation->health = UMI_EVIDENCE_UNKNOWN;
    /* Use the stable identifier comparison to choose the matching record or policy. */
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

UMI_DEFINE_STAGED_VALUE_INIT(umi_generation_init, UmiGeneration, generation,
    (UmiGeneration *generation, uint64_t number, const char *release_id, const char *root),
    umi_generation_init_build_value(candidate, number, release_id, root))


/*
 * Provide the generation mark active operation used by this module and its client
 * applications.
 */
void umi_generation_mark_active(UmiGeneration *generation, int active)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (generation != NULL) generation->active = active != 0;
}
