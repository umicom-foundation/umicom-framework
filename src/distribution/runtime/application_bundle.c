/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/application_bundle.c
 *
 * PURPOSE:
 *   application bundle metadata, selected variant and immutable content fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/application_bundle.h"

/*
 * Initialise dr application bundle from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_application_bundle_init(UmiDrApplicationBundle *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrApplicationBundle){0};  } }
/*
 * Check that dr application bundle satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_application_bundle_valid(const UmiDrApplicationBundle *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->variant_id[0] != '\0' && value->file_count>0U); }
/*
 * Provide the dr application bundle fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_application_bundle_fingerprint(const UmiDrApplicationBundle *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_application_bundle_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
