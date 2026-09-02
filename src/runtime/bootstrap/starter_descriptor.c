/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/starter_descriptor.c
 *
 * PURPOSE:
 *   Implement the starter descriptor behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/starter_descriptor.c
 *
 * PURPOSE:
 *   Describe reusable application starters built from feature packs and auto-configurations.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/starter_descriptor.h"


#include <string.h>
/*
 * Initialise bootstrap starter descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_bootstrap_starter_descriptor_init(
    UmiBootstrapStarterDescriptor *starter,
    const char *starter_id,
    const char *feature_pack_id,
    int32_t priority) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (starter == NULL || !umi_bootstrap_id_valid(starter_id) ||
        !umi_bootstrap_id_valid(feature_pack_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(starter, 0, sizeof(*starter));
    status = umi_bootstrap_copy_text(starter->starter_id, sizeof(starter->starter_id), starter_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(starter->feature_pack_id,
        sizeof(starter->feature_pack_id), feature_pack_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    starter->priority = priority;
    starter->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Provide the bootstrap starter descriptor add auto configuration operation used by this
 * module and its client applications.
 */
UmiStatus umi_bootstrap_starter_descriptor_add_auto_configuration(
    UmiBootstrapStarterDescriptor *starter,
    const char *configuration_id) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (starter == NULL || !umi_bootstrap_id_valid(configuration_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < starter->auto_configuration_count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(starter->auto_configuration_ids[i], configuration_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (starter->auto_configuration_count >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)umi_bootstrap_copy_text(
        starter->auto_configuration_ids[starter->auto_configuration_count++],
        UMI_BOOTSTRAP_ID_CAPACITY, configuration_id);
    return UMI_STATUS_OK;
}
