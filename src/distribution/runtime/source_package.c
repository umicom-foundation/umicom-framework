/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/source_package.c
 *
 * PURPOSE:
 *   source distribution metadata, licence and reproducibility flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/source_package.h"

void umi_dr_source_package_init(UmiDrSourcePackage *value) { if (value != NULL) { *value = (UmiDrSourcePackage){0};  } }
bool umi_dr_source_package_valid(const UmiDrSourcePackage *value) { return value != NULL && (value->id[0] != '\0' && value->revision[0] != '\0' && value->licence[0] != '\0'); }
uint64_t umi_dr_source_package_fingerprint(const UmiDrSourcePackage *value) {
    uint64_t h = 0U;
    if (!umi_dr_source_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
