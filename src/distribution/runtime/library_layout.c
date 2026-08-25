/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/library_layout.c
 *
 * PURPOSE:
 *   shared/private runtime library placement policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/library_layout.h"

void umi_dr_library_layout_init(UmiDrLibraryLayout *value) { if (value != NULL) { *value = (UmiDrLibraryLayout){0};  } }
bool umi_dr_library_layout_valid(const UmiDrLibraryLayout *value) { return value != NULL && (value->id[0] != '\0' && value->private_dir[0] != '\0' && value->search_relative); }
uint64_t umi_dr_library_layout_fingerprint(const UmiDrLibraryLayout *value) {
    uint64_t h = 0U;
    if (!umi_dr_library_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
