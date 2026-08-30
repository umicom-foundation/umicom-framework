/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/provenance_chain.h
 *
 * PURPOSE:
 *   Maintain and verify a deterministic chain of package provenance evidence.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_PROVENANCE_CHAIN_H
#define UMICOM_PLUGIN_EXTENSION_HOST_PROVENANCE_CHAIN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostProvenanceChainEntry { uint64_t evidence; uint64_t previous; uint64_t chain_hash; } UmiPluginExtensionHostProvenanceChainEntry;
typedef struct UmiPluginExtensionHostProvenanceChain { UmiPluginExtensionHostProvenanceChainEntry entries[UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY]; size_t count; } UmiPluginExtensionHostProvenanceChain;
void umi_plugin_extension_host_provenance_chain_init(UmiPluginExtensionHostProvenanceChain *chain);
UmiStatus umi_plugin_extension_host_provenance_chain_append(UmiPluginExtensionHostProvenanceChain *chain, uint64_t evidence);
int umi_plugin_extension_host_provenance_chain_verify(const UmiPluginExtensionHostProvenanceChain *chain);
uint64_t umi_plugin_extension_host_provenance_chain_head(const UmiPluginExtensionHostProvenanceChain *chain);

#ifdef __cplusplus
}
#endif

#endif
