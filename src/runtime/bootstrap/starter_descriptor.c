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
UmiStatus umi_bootstrap_starter_descriptor_init(
    UmiBootstrapStarterDescriptor *starter,
    const char *starter_id,
    const char *feature_pack_id,
    int32_t priority) {
    UmiStatus status;
    if (starter == NULL || !umi_bootstrap_id_valid(starter_id) ||
        !umi_bootstrap_id_valid(feature_pack_id)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(starter, 0, sizeof(*starter));
    status = umi_bootstrap_copy_text(starter->starter_id, sizeof(starter->starter_id), starter_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_bootstrap_copy_text(starter->feature_pack_id,
        sizeof(starter->feature_pack_id), feature_pack_id);
    if (status != UMI_STATUS_OK) return status;
    starter->priority = priority;
    starter->enabled = true;
    return UMI_STATUS_OK;
}
UmiStatus umi_bootstrap_starter_descriptor_add_auto_configuration(
    UmiBootstrapStarterDescriptor *starter,
    const char *configuration_id) {
    size_t i;
    if (starter == NULL || !umi_bootstrap_id_valid(configuration_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < starter->auto_configuration_count; ++i)
        if (strcmp(starter->auto_configuration_ids[i], configuration_id) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (starter->auto_configuration_count >= UMI_BOOTSTRAP_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)umi_bootstrap_copy_text(
        starter->auto_configuration_ids[starter->auto_configuration_count++],
        UMI_BOOTSTRAP_ID_CAPACITY, configuration_id);
    return UMI_STATUS_OK;
}
