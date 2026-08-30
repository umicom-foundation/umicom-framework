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

/* BEGINNER NOTE:
 * Update and rollback decisions need one authoritative view of what is installed and whether it is healthy.
 */

#include "umicom/delivery/install_state.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_install_state_init(UmiInstallState *state,
                                 const char *application_id,
                                 const char *version,
                                 uint64_t generation,
                                 const char *install_root)
{
    if (state == NULL || application_id == NULL || version == NULL ||
        install_root == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(state, 0, sizeof(*state));
    state->generation = generation;
    if (umi_delivery_copy_text(state->application_id, sizeof(state->application_id), application_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(state->version, sizeof(state->version), version) != UMI_STATUS_OK ||
        umi_delivery_copy_text(state->install_root, sizeof(state->install_root), install_root) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
