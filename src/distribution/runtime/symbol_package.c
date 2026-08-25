/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/symbol_package.c
 *
 * PURPOSE:
 *   debug symbol package metadata and build-id matching.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/symbol_package.h"

void umi_dr_symbol_package_init(UmiDrSymbolPackage *value) { if (value != NULL) { *value = (UmiDrSymbolPackage){0};  } }
bool umi_dr_symbol_package_valid(const UmiDrSymbolPackage *value) { return value != NULL && (value->id[0] != '\0' && value->build_id[0] != '\0' && value->digest[0] != '\0'); }
uint64_t umi_dr_symbol_package_fingerprint(const UmiDrSymbolPackage *value) {
    uint64_t h = 0U;
    if (!umi_dr_symbol_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
