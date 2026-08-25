/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/configuration_layout.c
 *
 * PURPOSE:
 *   system, user and portable configuration-root policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/configuration_layout.h"

void umi_dr_configuration_layout_init(UmiDrConfigurationLayout *value) { if (value != NULL) { *value = (UmiDrConfigurationLayout){0};  } }
bool umi_dr_configuration_layout_valid(const UmiDrConfigurationLayout *value) { return value != NULL && (value->id[0] != '\0' && value->user_root[0] != '\0' && (value->portable || value->system_root[0] != '\0')); }
uint64_t umi_dr_configuration_layout_fingerprint(const UmiDrConfigurationLayout *value) {
    uint64_t h = 0U;
    if (!umi_dr_configuration_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
