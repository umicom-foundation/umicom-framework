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
UmiStatus umi_bootstrap_auto_configuration_init(
    UmiBootstrapAutoConfiguration *configuration,
    const char *configuration_id,
    int32_t priority,
    UmiBootstrapConditionMode condition_mode);

#ifdef __cplusplus
}
#endif

#endif
