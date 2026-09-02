/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/provenance.c
 *
 * PURPOSE:
 *   Record source revision, builder identity and build inputs for release provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Provenance links a package back to the exact source and build environment that created it.
 */

#include "umicom/delivery/provenance.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise provenance from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_provenance_init(UmiProvenance *provenance,
                              const char *source_revision,
                              const char *builder_id,
                              const char *build_preset)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provenance == NULL || source_revision == NULL || builder_id == NULL ||
        build_preset == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(provenance, 0, sizeof(*provenance));
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_delivery_copy_text(provenance->source_revision,
                               sizeof(provenance->source_revision),
                               source_revision) != UMI_STATUS_OK ||
        umi_delivery_copy_text(provenance->builder_id,
                               sizeof(provenance->builder_id),
                               builder_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(provenance->build_preset,
                               sizeof(provenance->build_preset),
                               build_preset) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Check that provenance satisfies its contract before another service relies on it. */
UmiStatus umi_provenance_validate(const UmiProvenance *provenance)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provenance == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return provenance->source_revision[0] != '\0' &&
           provenance->builder_id[0] != '\0'
               ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
