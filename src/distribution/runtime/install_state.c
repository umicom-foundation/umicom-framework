/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/install_state.c
 *
 * PURPOSE:
 *   installed application version, channel and health state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/install_state.h"

/*
 * Initialise dr install state from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_install_state_init(UmiDrInstallState *value) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value != NULL) { *value = (UmiDrInstallState){0};  } }
/* Check that dr install state satisfies its contract before another service relies on it. */
bool umi_dr_install_state_valid(const UmiDrInstallState *value) { return value != NULL && (value->id[0] != '\0' && value->application_id[0] != '\0' && value->channel != 0 && value->scope != 0); }
/*
 * Provide the dr install state fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_install_state_fingerprint(const UmiDrInstallState *value) {
    uint64_t h = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_dr_install_state_valid(value)) return 0U;
    h = umi_dr_hash_combine(h, umi_dr_hash_text((const char *)value->id));
    h = umi_dr_hash_combine(h, (uint64_t)sizeof(*value));
    return h;
}
