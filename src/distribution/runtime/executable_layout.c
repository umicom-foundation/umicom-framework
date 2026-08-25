/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/executable_layout.c
 *
 * PURPOSE:
 *   executable placement and launch-entry validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/executable_layout.h"

void umi_dr_executable_layout_init(UmiDrExecutableLayout *value) { if (value != NULL) { *value = (UmiDrExecutableLayout){0};  } }
bool umi_dr_executable_layout_valid(const UmiDrExecutableLayout *value) { return value != NULL && (value->id[0] != '\0' && value->entrypoint[0] != '\0' && value->bin_dir[0] != '\0'); }
uint64_t umi_dr_executable_layout_fingerprint(const UmiDrExecutableLayout *value) {
    uint64_t h = 0U;
    if (!umi_dr_executable_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
