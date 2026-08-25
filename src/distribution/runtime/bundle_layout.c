/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/bundle_layout.c
 *
 * PURPOSE:
 *   portable application bundle directory layout validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_layout.h"

void umi_dr_bundle_layout_init(UmiDrBundleLayout *value) { if (value != NULL) { *value = (UmiDrBundleLayout){0};  } }
bool umi_dr_bundle_layout_valid(const UmiDrBundleLayout *value) { return value != NULL && (value->id[0] != '\0' && value->bin_dir[0] != '\0' && value->lib_dir[0] != '\0' && value->share_dir[0] != '\0'); }
uint64_t umi_dr_bundle_layout_fingerprint(const UmiDrBundleLayout *value) {
    uint64_t h = 0U;
    if (!umi_dr_bundle_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
