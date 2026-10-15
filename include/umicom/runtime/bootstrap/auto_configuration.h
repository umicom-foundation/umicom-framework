/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/auto_configuration.h
 *
 * PURPOSE:
 *   Describe conditional Framework-owned auto-configuration contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_H
#define UMICOM_RUNTIME_BOOTSTRAP_AUTO_CONFIGURATION_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap auto configuration from caller-provided values so later operations
 * receive a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_bootstrap_auto_configuration_init(
    UmiBootstrapAutoConfiguration *configuration,
    const char *configuration_id,
    int32_t priority,
    UmiBootstrapConditionMode condition_mode);

#ifdef __cplusplus
}
#endif

#endif
