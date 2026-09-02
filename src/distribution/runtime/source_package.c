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

/*
 * Initialise dr source package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_source_package_init(UmiDrSourcePackage *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrSourcePackage){0};  } }
/* Check that dr source package satisfies its contract before another service relies on it. */
bool umi_dr_source_package_valid(const UmiDrSourcePackage *value) { return value != NULL && (value->id[0] != '\0' && value->revision[0] != '\0' && value->licence[0] != '\0'); }
/*
 * Provide the dr source package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_source_package_fingerprint(const UmiDrSourcePackage *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_source_package_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
