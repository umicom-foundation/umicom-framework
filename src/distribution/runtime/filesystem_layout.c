/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/filesystem_layout.c
 *
 * PURPOSE:
 *   canonical install-root, bin, lib, share and writable-state layout.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/filesystem_layout.h"

void umi_dr_filesystem_layout_init(UmiDrFilesystemLayout *value) { if (value != NULL) { *value = (UmiDrFilesystemLayout){0};  } }
bool umi_dr_filesystem_layout_valid(const UmiDrFilesystemLayout *value) { return value != NULL && (value->id[0] != '\0' && value->root[0] != '\0' && value->bin[0] != '\0' && value->lib[0] != '\0'); }
uint64_t umi_dr_filesystem_layout_fingerprint(const UmiDrFilesystemLayout *value) {
    uint64_t h = 0U;
    if (!umi_dr_filesystem_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
