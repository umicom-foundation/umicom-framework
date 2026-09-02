/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/symbol_package.c
 *
 * PURPOSE:
 *   debug symbol package metadata and build-id matching.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/symbol_package.h"

/*
 * Initialise dr symbol package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_symbol_package_init(UmiDrSymbolPackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrSymbolPackage){0};  } }
/* Check that dr symbol package satisfies its contract before another service relies on it. */
bool umi_dr_symbol_package_valid(const UmiDrSymbolPackage *value) { return value != NULL && (value->id[0] != '\0' && value->build_id[0] != '\0' && value->digest[0] != '\0'); }
/*
 * Provide the dr symbol package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_symbol_package_fingerprint(const UmiDrSymbolPackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_symbol_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
