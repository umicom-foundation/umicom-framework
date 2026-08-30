/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/trust_store.h
 *
 * PURPOSE:
 *   Maintain bounded trust decisions by extension or publisher identity.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_TRUST_STORE_H
#define UMICOM_PLUGIN_EXTENSION_HOST_TRUST_STORE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostTrustStoreItem { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; uint32_t state; uint32_t priority; uint64_t evidence; } UmiPluginExtensionHostTrustStoreItem;
typedef struct UmiPluginExtensionHostTrustStore { UmiPluginExtensionHostTrustStoreItem items[UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY]; size_t count; uint64_t revision; } UmiPluginExtensionHostTrustStore;

void umi_plugin_extension_host_trust_store_init(UmiPluginExtensionHostTrustStore *catalogue);
UmiStatus umi_plugin_extension_host_trust_store_add(UmiPluginExtensionHostTrustStore *catalogue, const char *id, uint32_t state, uint32_t priority, uint64_t evidence);
const UmiPluginExtensionHostTrustStoreItem *umi_plugin_extension_host_trust_store_find(const UmiPluginExtensionHostTrustStore *catalogue, const char *id);
UmiStatus umi_plugin_extension_host_trust_store_remove(UmiPluginExtensionHostTrustStore *catalogue, const char *id);
size_t umi_plugin_extension_host_trust_store_count(const UmiPluginExtensionHostTrustStore *catalogue);

#ifdef __cplusplus
}
#endif

#endif
