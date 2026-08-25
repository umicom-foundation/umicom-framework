/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/install_state.c
 *
 * PURPOSE:
 *   installed application version, channel and health state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/install_state.h"

void umi_dr_install_state_init(UmiDrInstallState *value) { if (value != NULL) { *value = (UmiDrInstallState){0};  } }
bool umi_dr_install_state_valid(const UmiDrInstallState *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->channel != 0 && value->scope != 0); }
uint64_t umi_dr_install_state_fingerprint(const UmiDrInstallState *value) {
    uint64_t h = 0U;
    if (!umi_dr_install_state_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
