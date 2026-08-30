/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/auto_configuration.c
 *
 * PURPOSE:
 *   Implement the auto configuration behavior for
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
 * File: src/runtime/bootstrap/auto_configuration.c
 *
 * PURPOSE:
 *   Describe conditional Framework-owned auto-configuration contributions.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/auto_configuration.h"
#include "umicom/runtime/bootstrap/condition_set.h"


#include <string.h>
UmiStatus umi_bootstrap_auto_configuration_init(
    UmiBootstrapAutoConfiguration *configuration,
    const char *configuration_id,
    int32_t priority,
    UmiBootstrapConditionMode condition_mode) {
    UmiStatus status;
    if (configuration == NULL || !umi_bootstrap_id_valid(configuration_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(configuration, 0, sizeof(*configuration));
    status = umi_bootstrap_copy_text(configuration->configuration_id,
        sizeof(configuration->configuration_id), configuration_id);
    if (status != UMI_STATUS_OK) return status;
    configuration->priority = priority;
    configuration->enabled = true;
    umi_bootstrap_condition_set_init(&configuration->conditions, condition_mode);
    return UMI_STATUS_OK;
}
