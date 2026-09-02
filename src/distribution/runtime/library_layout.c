/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/library_layout.c
 *
 * PURPOSE:
 *   shared/private runtime library placement policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/library_layout.h"

/*
 * Initialise dr library layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_library_layout_init(UmiDrLibraryLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrLibraryLayout){0};  } }
/* Check that dr library layout satisfies its contract before another service relies on it. */
bool umi_dr_library_layout_valid(const UmiDrLibraryLayout *value) { return value != NULL && (value->id[0] != '\0' && value->private_dir[0] != '\0' && value->search_relative); }
/*
 * Provide the dr library layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_library_layout_fingerprint(const UmiDrLibraryLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_library_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
