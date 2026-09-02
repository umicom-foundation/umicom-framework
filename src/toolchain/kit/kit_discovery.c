/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/kit/kit_discovery.c
 * PURPOSE: Compose compiler catalogue, SDK discovery and runtimes into kits.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/kit_discovery.h"

#include <string.h>

#include "umicom/toolchain/python_runtime.h"
#include "umicom/toolchain/sdk.h"

/*
 * Provide the first available sdk operation used by this module and its client
 * applications.
 */
static const UmiSdkSnapshot *first_available_sdk(
    const UmiSdkCatalogueSnapshot *catalogue)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (catalogue->items[index].available != 0) return &catalogue->items[index];
    }
    return NULL;
}

/*
 * Initialise toolchain kit discovery request from caller-provided values so later
 * operations receive a known state.
 */
void umi_toolchain_kit_discovery_request_init(
    UmiToolchainKitDiscoveryRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->struct_size = (uint32_t)sizeof(*request);
    request->api_version = UMI_TOOLCHAIN_KIT_DISCOVERY_API_VERSION;
}

/*
 * Provide the toolchain kit discover catalogue operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_discover_catalogue(
    const UmiToolchainCatalogue *toolchains,
    UmiToolchainKitCatalogue *kits,
    const UmiToolchainKitDiscoveryRequest *request,
    UmiToolchainKitDiscoverySnapshot *out_snapshot)
{
    UmiPythonRuntimeSnapshot python;
    UmiPythonRuntimeRequest python_request;
    size_t index;
    int python_available = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (toolchains == NULL || kits == NULL || request == NULL ||
        out_snapshot == NULL || request->struct_size < sizeof(*request) ||
        request->api_version != UMI_TOOLCHAIN_KIT_DISCOVERY_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TOOLCHAIN_KIT_DISCOVERY_API_VERSION;
    (void)memset(&python, 0, sizeof(python));

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->discover_python != 0) {
        umi_python_runtime_request_init(&python_request);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_python_runtime_discover(&python_request, &python) == UMI_STATUS_OK) {
            python_available = 1;
            out_snapshot->python_available = 1;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_toolchain_catalogue_profile_count(toolchains); ++index) {
        UmiToolchainProfile profile;
        UmiSdkCatalogueSnapshot sdk_catalogue;
        const UmiSdkSnapshot *sdk = NULL;
        UmiToolchainKitSnapshot kit;
        UmiStatus status;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_toolchain_catalogue_profile_at(toolchains, index, &profile) !=
            UMI_STATUS_OK) continue;
        out_snapshot->attempted_profiles += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (profile.complete == 0 && request->include_incomplete_profiles == 0)
            continue;
        (void)memset(&sdk_catalogue, 0, sizeof(sdk_catalogue));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_toolchain_sdk_discover(&profile, &sdk_catalogue) == UMI_STATUS_OK)
            sdk = first_available_sdk(&sdk_catalogue);
        status = umi_toolchain_kit_from_profile(&kit, &profile, sdk,
                                                request->host_triple);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (python_available != 0)
            (void)umi_toolchain_kit_apply_python_runtime(&kit, &python);
        status = umi_toolchain_kit_catalogue_upsert(kits, &kit);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        out_snapshot->kits_created += 1U;
        /* Apply this operation only while the related capability or state is available. */
        if (kit.state == UMI_TOOLCHAIN_KIT_READY)
            out_snapshot->ready_kits += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (kit.state == UMI_TOOLCHAIN_KIT_DEGRADED)
            out_snapshot->degraded_kits += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((kit.capabilities & UMI_TOOLCHAIN_KIT_CAPABILITY_SDK) != 0U)
            out_snapshot->sdk_backed_kits += 1U;
    }
    return UMI_STATUS_OK;
}
