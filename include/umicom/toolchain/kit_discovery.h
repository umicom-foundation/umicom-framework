/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_discovery.h
 * PURPOSE: Compose discovered compiler profiles, SDKs and runtimes into kits.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_DISCOVERY_H
#define UMICOM_TOOLCHAIN_KIT_DISCOVERY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/toolchain/catalogue.h"
#include "umicom/toolchain/kit_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_KIT_DISCOVERY_API_VERSION 1U

typedef struct UmiToolchainKitDiscoveryRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *host_triple;
    int include_incomplete_profiles;
    int discover_python;
} UmiToolchainKitDiscoveryRequest;

typedef struct UmiToolchainKitDiscoverySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t attempted_profiles;
    size_t kits_created;
    size_t ready_kits;
    size_t degraded_kits;
    size_t sdk_backed_kits;
    int python_available;
} UmiToolchainKitDiscoverySnapshot;

void umi_toolchain_kit_discovery_request_init(
    UmiToolchainKitDiscoveryRequest *request);
UmiStatus umi_toolchain_kit_discover_catalogue(
    const UmiToolchainCatalogue *toolchains,
    UmiToolchainKitCatalogue *kits,
    const UmiToolchainKitDiscoveryRequest *request,
    UmiToolchainKitDiscoverySnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
