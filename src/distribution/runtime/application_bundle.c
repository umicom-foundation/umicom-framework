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

void umi_dr_application_bundle_init(UmiDrApplicationBundle *value) { if (value != NULL) { *value = (UmiDrApplicationBundle){0};  } }
bool umi_dr_application_bundle_valid(const UmiDrApplicationBundle *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->variant_id[0] != '\0' && value->file_count>0U); }
uint64_t umi_dr_application_bundle_fingerprint(const UmiDrApplicationBundle *value) {
    uint64_t h = 0U;
    if (!umi_dr_application_bundle_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
