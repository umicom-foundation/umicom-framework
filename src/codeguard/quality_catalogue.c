/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/quality_catalogue.c
 *
 * PURPOSE:
 *   Implement the quality catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Architecture-quality capabilities | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/quality_catalogue.h"
#include <stddef.h>
static const char *CAPABILITIES[] = {
    "umicom.codeguard.evidence/2",
    "umicom.codeguard.architecture-policy/2",
    "umicom.codeguard.memory-audit/2",
    "umicom.codeguard.portability-audit/2",
    "umicom.codeguard.unsafe-api/2",
    "umicom.codeguard.duplicate-evidence/2",
    "umicom.codeguard.api-baseline/2",
    "umicom.codeguard.combined-quality-gate/2",
    "umicom.codeguard.remediation-plan/2"
};
size_t umi_codeguard_quality_catalogue_count(void) { return sizeof(CAPABILITIES) / sizeof(CAPABILITIES[0]); }
const char *umi_codeguard_quality_catalogue_at(size_t index) { return index < umi_codeguard_quality_catalogue_count() ? CAPABILITIES[index] : NULL; }
