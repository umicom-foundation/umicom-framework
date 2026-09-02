/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/quarantine_policy.h
 *
 * PURPOSE:
 *   Determine when repeated crashes, trust failures or signature failures require quarantine.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_QUARANTINE_POLICY_H
#define UMICOM_PLUGIN_EXTENSION_HOST_QUARANTINE_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host quarantine policy data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostQuarantinePolicy { uint32_t crash_threshold; uint32_t trust_failure_threshold; uint32_t signature_failure_threshold; uint32_t policy_failure_threshold; } UmiPluginExtensionHostQuarantinePolicy;
/**
 * Initialise plugin extension host quarantine policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_quarantine_policy_init(UmiPluginExtensionHostQuarantinePolicy *policy);
/**
 * Provide the plugin extension host quarantine policy should quarantine operation used by
 * this module and its client applications.
 */
int umi_plugin_extension_host_quarantine_policy_should_quarantine(const UmiPluginExtensionHostQuarantinePolicy *policy, uint32_t crashes, uint32_t trust_failures, uint32_t signature_failures, uint32_t policy_failures);

#ifdef __cplusplus
}
#endif

#endif
