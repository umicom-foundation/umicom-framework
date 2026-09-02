/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/data_layout.c
 *
 * PURPOSE:
 *   read-only packaged data and writable application-data separation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/data_layout.h"

/*
 * Initialise dr data layout from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_data_layout_init(UmiDrDataLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrDataLayout){0};  } }
/* Check that dr data layout satisfies its contract before another service relies on it. */
bool umi_dr_data_layout_valid(const UmiDrDataLayout *value) { return value != NULL && (value->id[0] != '\0' && value->read_only_dir[0] != '\0' && value->writable_dir[0] != '\0'); }
/*
 * Provide the dr data layout fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_data_layout_fingerprint(const UmiDrDataLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_data_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
