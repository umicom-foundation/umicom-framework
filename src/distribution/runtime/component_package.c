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

/*
 * Initialise dr component package from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_component_package_init(UmiDrComponentPackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrComponentPackage){0};  } }
/*
 * Check that dr component package satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_component_package_valid(const UmiDrComponentPackage *value) { return value != NULL && (value->id[0] != '\0' && value->component_id[0] != '\0'); }
/*
 * Provide the dr component package fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_component_package_fingerprint(const UmiDrComponentPackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_component_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
