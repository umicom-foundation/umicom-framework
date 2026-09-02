/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/runtime_dependency.c
 *
 * PURPOSE:
 *   native runtime-library dependency and availability policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_dependency.h"

/*
 * Initialise dr runtime dependency from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_runtime_dependency_init(UmiDrRuntimeDependency *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrRuntimeDependency){0};  } }
/*
 * Check that dr runtime dependency satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_runtime_dependency_valid(const UmiDrRuntimeDependency *value) { return value != NULL && (value->id[0] != '\0' && value->soname[0] != '\0' && (value->system_provided || value->bundled)); }
/*
 * Provide the dr runtime dependency fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_runtime_dependency_fingerprint(const UmiDrRuntimeDependency *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_runtime_dependency_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
