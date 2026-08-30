/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/component_package.c
 *
 * PURPOSE:
 *   optional Framework component package and dependency metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/component_package.h"

void umi_dr_component_package_init(UmiDrComponentPackage *value) { if (value != NULL) { *value = (UmiDrComponentPackage){0};  } }
bool umi_dr_component_package_valid(const UmiDrComponentPackage *value) { return value != NULL && (value->id[0] != '\0' && value->component_id[0] != '\0'); }
uint64_t umi_dr_component_package_fingerprint(const UmiDrComponentPackage *value) {
    uint64_t h = 0U;
    if (!umi_dr_component_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
