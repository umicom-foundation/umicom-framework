/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/update_candidate.h
 *
 * PURPOSE:
 *   Describe one extension update candidate and compatibility/risk evidence.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_UPDATE_CANDIDATE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_UPDATE_CANDIDATE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host update candidate data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostUpdateCandidate {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostUpdateCandidate;

/**
 * Initialise plugin extension host update candidate from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_update_candidate_init(UmiPluginExtensionHostUpdateCandidate *value);
/**
 * Provide the plugin extension host update candidate configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_update_candidate_configure(UmiPluginExtensionHostUpdateCandidate *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host update candidate satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_plugin_extension_host_update_candidate_validate(const UmiPluginExtensionHostUpdateCandidate *value);
/**
 * Provide the plugin extension host update candidate fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_update_candidate_fingerprint(const UmiPluginExtensionHostUpdateCandidate *value);

#ifdef __cplusplus
}
#endif

#endif
