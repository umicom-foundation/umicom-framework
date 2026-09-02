/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/provenance.c
 *
 * PURPOSE:
 *   build/source/toolchain provenance evidence for packaged releases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/provenance.h"

/*
 * Initialise dr provenance from caller-provided values so later operations receive a known
 * state.
 */
void umi_dr_provenance_init(UmiDrProvenance *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrProvenance){0};  } }
/* Check that dr provenance satisfies its contract before another service relies on it. */
bool umi_dr_provenance_valid(const UmiDrProvenance *value) { return value != NULL && (value->id[0] != '\0' && value->source_revision[0] != '\0' && value->toolchain[0] != '\0'); }
/*
 * Provide the dr provenance fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_provenance_fingerprint(const UmiDrProvenance *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_provenance_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
