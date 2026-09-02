/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/contract.c
 *
 * PURPOSE:
 *   Implement explicit version, ABI and module descriptor validation contracts
 *   for reusable Framework application and plug-in boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/contract.h"

#include <string.h>

/* Provide the present text operation used by this module and its client applications. */
static int present_text(const char *text)
{
    return text != NULL && text[0] != '\0';
}

/* Check that module kind satisfies its contract before another service relies on it. */
static int module_kind_valid(UmiModuleKind kind)
{
    return kind >= UMI_MODULE_CORE && kind <= UMI_MODULE_AGENT;
}

/*
 * Provide the runtime contract snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_runtime_contract_snapshot(UmiRuntimeContractSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_RUNTIME_CONTRACT_API_VERSION;
    out_snapshot->framework_version = (UmiVersion){
        (uint16_t)UMICOM_FRAMEWORK_VERSION_MAJOR,
        (uint16_t)UMICOM_FRAMEWORK_VERSION_MINOR,
        (uint16_t)UMICOM_FRAMEWORK_VERSION_PATCH
    };
    out_snapshot->framework_abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    out_snapshot->command_descriptor_size = sizeof(UmiCommandDescriptor);
    out_snapshot->service_descriptor_size = sizeof(UmiServiceDescriptor);
    out_snapshot->capability_descriptor_size = sizeof(UmiCapabilityDescriptor);
    out_snapshot->module_lifecycle_size = sizeof(UmiModuleLifecycle);
    out_snapshot->module_descriptor_size = sizeof(UmiModuleDescriptor);
    out_snapshot->inventory_snapshot_size = sizeof(UmiRuntimeInventorySnapshot);
    return UMI_STATUS_OK;
}

/*
 * Provide the runtime contract version at least operation used by this module and its
 * client applications.
 */
int umi_runtime_contract_version_at_least(const UmiVersion *actual,
                                          const UmiVersion *minimum)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (actual == NULL || minimum == NULL) {
        return 0;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (actual->major != minimum->major) {
        return actual->major > minimum->major;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (actual->minor != minimum->minor) {
        return actual->minor > minimum->minor;
    }
    return actual->patch >= minimum->patch;
}

/*
 * Provide the runtime contract abi matches operation used by this module and its client
 * applications.
 */
int umi_runtime_contract_abi_matches(uint32_t abi_version)
{
    return abi_version == UMICOM_FRAMEWORK_ABI_VERSION;
}

/*
 * Check that runtime module contract satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_runtime_module_contract_validate(
    const UmiModuleDescriptor *descriptor,
    UmiModuleContractReport *out_report)
{
    int start_present;
    int stop_present;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->structure_size = (uint32_t)sizeof(*out_report);
    out_report->api_version = UMI_RUNTIME_CONTRACT_API_VERSION;
    out_report->structure_compatible =
        descriptor->structure_size >= sizeof(UmiModuleDescriptor);
    out_report->abi_compatible =
        umi_runtime_contract_abi_matches(descriptor->abi_version);
    out_report->identity_valid =
        present_text(descriptor->module_id) && present_text(descriptor->display_name);
    out_report->kind_valid = module_kind_valid(descriptor->kind);

    start_present = descriptor->lifecycle.start != NULL;
    stop_present = descriptor->lifecycle.stop != NULL;
    out_report->lifecycle_balanced =
        start_present == stop_present &&
        (descriptor->lifecycle.quiesce == NULL || stop_present);

    out_report->status =
        out_report->structure_compatible &&
        out_report->abi_compatible &&
        out_report->identity_valid &&
        out_report->kind_valid &&
        out_report->lifecycle_balanced
            ? UMI_STATUS_OK
            : UMI_STATUS_INVALID_STATE;

    return out_report->status;
}
