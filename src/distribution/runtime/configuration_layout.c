/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/configuration_layout.c
 *
 * PURPOSE:
 *   system, user and portable configuration-root policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/configuration_layout.h"

/*
 * Initialise dr configuration layout from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_configuration_layout_init(UmiDrConfigurationLayout *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrConfigurationLayout){0};  } }
/*
 * Check that dr configuration layout satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_configuration_layout_valid(const UmiDrConfigurationLayout *value) { return value != NULL && (value->id[0] != '\0' && value->user_root[0] != '\0' && (value->portable || value->system_root[0] != '\0')); }
/*
 * Provide the dr configuration layout fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_configuration_layout_fingerprint(const UmiDrConfigurationLayout *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_configuration_layout_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
