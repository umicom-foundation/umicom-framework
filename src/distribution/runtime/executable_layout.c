/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/executable_layout.c
 *
 * PURPOSE:
 *   executable placement and launch-entry validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/executable_layout.h"

/*
 * Initialise dr executable layout from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_executable_layout_init(UmiDrExecutableLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrExecutableLayout){0};  } }
/*
 * Check that dr executable layout satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_executable_layout_valid(const UmiDrExecutableLayout *value) { return value != NULL && (value->id[0] != '\0' && value->entrypoint[0] != '\0' && value->bin_dir[0] != '\0'); }
/*
 * Provide the dr executable layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_executable_layout_fingerprint(const UmiDrExecutableLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_executable_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
