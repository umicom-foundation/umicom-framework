/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/bundle_file.c
 *
 * PURPOSE:
 *   individual bundle-file path, size, checksum and executable metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_file.h"

void umi_dr_bundle_file_init(UmiDrBundleFile *value) { if (value != NULL) { *value = (UmiDrBundleFile){0};  } }
bool umi_dr_bundle_file_valid(const UmiDrBundleFile *value) { return value != NULL && (value->id[0] != '\0' && value->path[0] != '\0' && value->digest[0] != '\0' && value->size_bytes>0U); }
uint64_t umi_dr_bundle_file_fingerprint(const UmiDrBundleFile *value) {
    uint64_t h = 0U;
    if (!umi_dr_bundle_file_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
