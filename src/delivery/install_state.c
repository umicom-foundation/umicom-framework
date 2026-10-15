/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/install_state.c
 *
 * PURPOSE:
 *   Record the currently installed application version and active generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Update and rollback decisions need one authoritative view of what is installed and whether it is healthy.
 */

#include "umicom/delivery/install_state.h"
#include "../base/value_construction_internal.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise install state from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_install_state_init(UmiInstallState *state,
                                 const char *application_id,
                                 const char *version,
                                 uint64_t generation,
                                 const char *install_root)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || application_id == NULL || version == NULL ||
        install_root == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(state, 0, sizeof(*state));
    state->generation = generation;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(state->application_id, sizeof(state->application_id), application_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(state->version, sizeof(state->version), version) != UMI_STATUS_OK ||
        umi_delivery_copy_text(state->install_root, sizeof(state->install_root), install_root) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_install_state_init_build_value(UmiInstallState *state,
                                 const char *application_id,
                                 const char *version,
                                 uint64_t generation,
                                 const char *install_root)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || application_id == NULL || version == NULL ||
        install_root == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(state, 0, sizeof(*state));
    state->generation = generation;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(state->application_id, sizeof(state->application_id), application_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(state->version, sizeof(state->version), version) != UMI_STATUS_OK ||
        umi_delivery_copy_text(state->install_root, sizeof(state->install_root), install_root) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_install_state_init, UmiInstallState, state,
    (UmiInstallState *state, const char *application_id, const char *version, uint64_t generation, const char *install_root),
    umi_install_state_init_build_value(candidate, application_id, version, generation, install_root))
