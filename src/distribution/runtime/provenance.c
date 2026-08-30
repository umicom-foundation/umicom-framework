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

void umi_dr_provenance_init(UmiDrProvenance *value) { if (value != NULL) { *value = (UmiDrProvenance){0};  } }
bool umi_dr_provenance_valid(const UmiDrProvenance *value) { return value != NULL && (value->id[0] != '\0' && value->source_revision[0] != '\0' && value->toolchain[0] != '\0'); }
uint64_t umi_dr_provenance_fingerprint(const UmiDrProvenance *value) {
    uint64_t h = 0U;
    if (!umi_dr_provenance_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
