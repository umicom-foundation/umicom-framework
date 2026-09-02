/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/recovery_policy.h
 *
 * PURPOSE:
 *   Define bounded restart, backoff and checkpoint requirements after extension failure.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_RECOVERY_POLICY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_RECOVERY_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host recovery policy data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostRecoveryPolicy { uint32_t maximum_restarts; uint64_t base_backoff_ms; uint64_t maximum_backoff_ms; int require_checkpoint; int require_trust_revalidation; } UmiPluginExtensionHostRecoveryPolicy;
/**
 * Initialise plugin extension host recovery policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_recovery_policy_init(UmiPluginExtensionHostRecoveryPolicy *policy);
/**
 * Provide the plugin extension host recovery policy evaluate operation used by this module
 * and its client applications.
 */
UmiPluginExtensionHostDecision umi_plugin_extension_host_recovery_policy_evaluate(const UmiPluginExtensionHostRecoveryPolicy *policy, uint32_t restart_count, int checkpoint_available, int trust_valid);
/**
 * Provide the plugin extension host recovery policy backoff ms operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_recovery_policy_backoff_ms(const UmiPluginExtensionHostRecoveryPolicy *policy, uint32_t restart_count);

#ifdef __cplusplus
}
#endif

#endif
