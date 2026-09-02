/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/provenance_record.h
 *
 * PURPOSE:
 *   Describe one immutable package provenance hop.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PROVENANCE_RECORD_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PROVENANCE_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the plugin extension host provenance record data shared with callers of this
 * public contract.
 */
typedef struct UmiPluginExtensionHostProvenanceRecord {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY];
    char subject[UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY];
    uint32_t version;
    uint32_t risk;
    uint64_t flags;
    uint64_t revision;
} UmiPluginExtensionHostProvenanceRecord;

/**
 * Initialise plugin extension host provenance record from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_provenance_record_init(UmiPluginExtensionHostProvenanceRecord *value);
/**
 * Provide the plugin extension host provenance record configure operation used by this
 * module and its client applications.
 */
UmiStatus umi_plugin_extension_host_provenance_record_configure(UmiPluginExtensionHostProvenanceRecord *value, const char *id, const char *subject, uint32_t version, uint32_t risk, uint64_t flags);
/**
 * Check that plugin extension host provenance record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_plugin_extension_host_provenance_record_validate(const UmiPluginExtensionHostProvenanceRecord *value);
/**
 * Provide the plugin extension host provenance record fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_plugin_extension_host_provenance_record_fingerprint(const UmiPluginExtensionHostProvenanceRecord *value);

#ifdef __cplusplus
}
#endif

#endif
