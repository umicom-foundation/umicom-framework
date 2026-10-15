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
#include "../../base/value_construction_internal.h"
#include "umicom/runtime/bootstrap/condition_set.h"


#include <string.h>
/*
 * Initialise bootstrap auto configuration from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_bootstrap_auto_configuration_init(
    UmiBootstrapAutoConfiguration *configuration,
    const char *configuration_id,
    int32_t priority,
    UmiBootstrapConditionMode condition_mode) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (configuration == NULL || !umi_bootstrap_id_valid(configuration_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(configuration, 0, sizeof(*configuration));
    status = umi_bootstrap_copy_text(configuration->configuration_id,
        sizeof(configuration->configuration_id), configuration_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    configuration->priority = priority;
    configuration->enabled = true;
    umi_bootstrap_condition_set_init(&configuration->conditions, condition_mode);
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_bootstrap_auto_configuration_init_build_value(
    UmiBootstrapAutoConfiguration *configuration,
    const char *configuration_id,
    int32_t priority,
    UmiBootstrapConditionMode condition_mode) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (configuration == NULL || !umi_bootstrap_id_valid(configuration_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(configuration, 0, sizeof(*configuration));
    status = umi_bootstrap_copy_text(configuration->configuration_id,
        sizeof(configuration->configuration_id), configuration_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    configuration->priority = priority;
    configuration->enabled = true;
    umi_bootstrap_condition_set_init(&configuration->conditions, condition_mode);
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_bootstrap_auto_configuration_init, UmiBootstrapAutoConfiguration, configuration,
    (UmiBootstrapAutoConfiguration *configuration, const char *configuration_id, int32_t priority, UmiBootstrapConditionMode condition_mode),
    umi_bootstrap_auto_configuration_init_build_value(candidate, configuration_id, priority, condition_mode))
