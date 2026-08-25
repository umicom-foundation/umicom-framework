/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/data_layout.c
 *
 * PURPOSE:
 *   read-only packaged data and writable application-data separation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/data_layout.h"

void umi_dr_data_layout_init(UmiDrDataLayout *value) { if (value != NULL) { *value = (UmiDrDataLayout){0};  } }
bool umi_dr_data_layout_valid(const UmiDrDataLayout *value) { return value != NULL && (value->id[0] != '\0' && value->read_only_dir[0] != '\0' && value->writable_dir[0] != '\0'); }
uint64_t umi_dr_data_layout_fingerprint(const UmiDrDataLayout *value) {
    uint64_t h = 0U;
    if (!umi_dr_data_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
