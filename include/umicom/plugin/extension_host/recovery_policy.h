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

typedef struct UmiPluginExtensionHostRecoveryPolicy { uint32_t maximum_restarts; uint64_t base_backoff_ms; uint64_t maximum_backoff_ms; int require_checkpoint; int require_trust_revalidation; } UmiPluginExtensionHostRecoveryPolicy;
void umi_plugin_extension_host_recovery_policy_init(UmiPluginExtensionHostRecoveryPolicy *policy);
UmiPluginExtensionHostDecision umi_plugin_extension_host_recovery_policy_evaluate(const UmiPluginExtensionHostRecoveryPolicy *policy, uint32_t restart_count, int checkpoint_available, int trust_valid);
uint64_t umi_plugin_extension_host_recovery_policy_backoff_ms(const UmiPluginExtensionHostRecoveryPolicy *policy, uint32_t restart_count);

#ifdef __cplusplus
}
#endif

#endif
