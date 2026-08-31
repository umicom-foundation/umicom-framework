/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/quality_catalogue.c
 *
 * PURPOSE:
 *   Publish stable identifiers for reusable CodeGuard capabilities that
 *   applications can discover without copying implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/codeguard/quality_catalogue.h"

#include <stddef.h>

/* Each immutable identifier names one independently reusable quality contract. */
static const char *const CAPABILITIES[] = {
    "umicom.codeguard.evidence/2",
    "umicom.codeguard.architecture-policy/2",
    "umicom.codeguard.memory-audit/2",
    "umicom.codeguard.portability-audit/2",
    "umicom.codeguard.unsafe-api/2",
    "umicom.codeguard.duplicate-evidence/2",
    "umicom.codeguard.api-baseline/2",
    "umicom.codeguard.combined-quality-gate/2",
    "umicom.codeguard.remediation-plan/2",
    "umicom.codeguard.documentation-inventory/1"
};

/* Hide storage details while exposing the complete fixed catalogue size. */
size_t umi_codeguard_quality_catalogue_count(void)
{
    return sizeof(CAPABILITIES) / sizeof(CAPABILITIES[0]);
}

/* Borrow one identifier safely without transferring ownership to the caller. */
const char *umi_codeguard_quality_catalogue_at(size_t index)
{
    /* Bounds checking prevents reading beyond the fixed capability array. */
    if (index >= umi_codeguard_quality_catalogue_count()) {
        return NULL;
    }
    return CAPABILITIES[index];
}
