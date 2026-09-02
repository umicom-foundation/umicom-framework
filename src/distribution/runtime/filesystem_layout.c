/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/filesystem_layout.c
 *
 * PURPOSE:
 *   canonical install-root, bin, lib, share and writable-state layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/filesystem_layout.h"

/*
 * Initialise dr filesystem layout from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_filesystem_layout_init(UmiDrFilesystemLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrFilesystemLayout){0};  } }
/*
 * Check that dr filesystem layout satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_filesystem_layout_valid(const UmiDrFilesystemLayout *value) { return value != NULL && (value->id[0] != '\0' && value->root[0] != '\0' && value->bin[0] != '\0' && value->lib[0] != '\0'); }
/*
 * Provide the dr filesystem layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_filesystem_layout_fingerprint(const UmiDrFilesystemLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_filesystem_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
