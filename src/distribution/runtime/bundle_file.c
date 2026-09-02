/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/bundle_file.c
 *
 * PURPOSE:
 *   individual bundle-file path, size, checksum and executable metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_file.h"

/*
 * Initialise dr bundle file from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_bundle_file_init(UmiDrBundleFile *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrBundleFile){0};  } }
/* Check that dr bundle file satisfies its contract before another service relies on it. */
bool umi_dr_bundle_file_valid(const UmiDrBundleFile *value) { return value != NULL && (value->id[0] != '\0' && value->path[0] != '\0' && value->digest[0] != '\0' && value->size_bytes>0U); }
/*
 * Provide the dr bundle file fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_bundle_file_fingerprint(const UmiDrBundleFile *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_bundle_file_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
